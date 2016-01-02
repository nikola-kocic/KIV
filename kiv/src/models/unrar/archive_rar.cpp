#include "kiv/src/models/unrar/archive_rar.h"

#include <QBuffer>
#include <QDir>

#include "kiv/src/models/unrar/unrar.h"

#define  LHD_LARGE          0x0100
#define  LHD_UNICODE        0x0200
#define  LHD_SALT           0x0400
#define  LHD_EXTTIME        0x1000
#define  LHD_WINDOWMASK     0x00e0
#define  LHD_DIRECTORY      0x00e0
#define  LONG_BLOCK         0x8000

RAROpenArchiveExT      RAROpenArchiveEx = nullptr;
RARCloseArchiveT       RARCloseArchive = nullptr;
RARReadHeaderExT       RARReadHeaderEx = nullptr;
RARProcessFileWT       RARProcessFileW = nullptr;
RARSetCallbackT        RARSetCallback = nullptr;
RARSetChangeVolProcT   RARSetChangeVolProc = nullptr;
RARSetProcessDataProcT RARSetProcessDataProc = nullptr;
RARSetPasswordT        RARSetPassword = nullptr;
RARGetDllVersionT      RARGetDllVersion = nullptr;

struct RAROpenArchiveDataEx;

QLibrary *ArchiveRar::Lib = nullptr;

bool ArchiveRar::loadlib()
{
    if (Lib)
    {
        return true;
    }

    Lib = new QLibrary("unrar");
    if (!Lib->load())
    {
        Lib = nullptr;
        return false;
    }

    RAROpenArchiveEx = reinterpret_cast<RAROpenArchiveExT>(Lib->resolve("RAROpenArchiveEx"));
    RARCloseArchive = reinterpret_cast<RARCloseArchiveT>(Lib->resolve("RARCloseArchive"));
    RARReadHeaderEx = reinterpret_cast<RARReadHeaderExT>(Lib->resolve("RARReadHeaderEx"));
    RARProcessFileW = reinterpret_cast<RARProcessFileWT>(Lib->resolve("RARProcessFileW"));
    RARSetCallback = reinterpret_cast<RARSetCallbackT>(Lib->resolve("RARSetCallback"));
    RARSetChangeVolProc = reinterpret_cast<RARSetChangeVolProcT>(
                Lib->resolve("RARSetChangeVolProc"));
    RARSetProcessDataProc = reinterpret_cast<RARSetProcessDataProcT>(
                Lib->resolve("RARSetProcessDataProc"));
    RARSetPassword = reinterpret_cast<RARSetPasswordT>(Lib->resolve("RARSetPassword"));
    RARGetDllVersion = reinterpret_cast<RARGetDllVersionT>(Lib->resolve("RARGetDllVersion"));

    return true;
}

int ArchiveRar::extract(const QString &archiveName,
                                 const QString &fileName,
                                 const QString &newFileName)
{
    const std::wstring fileNameW =
            QDir::toNativeSeparators(fileName).toStdWString();
    const std::wstring arcNameW = archiveName.toStdWString();
    int returnCode = 1000;

    RAROpenArchiveDataEx OpenArchiveData;
    memset(&OpenArchiveData, 0, sizeof(OpenArchiveData));
    OpenArchiveData.ArcNameW = arcNameW.c_str();
    OpenArchiveData.CmtBufSize = 0;
    OpenArchiveData.OpenMode = RAR_OM_EXTRACT;
    OpenArchiveData.Callback = nullptr;
    OpenArchiveData.UserData = 0;

    Qt::HANDLE hArcData = RAROpenArchiveEx(&OpenArchiveData);

    if (OpenArchiveData.OpenResult != 0)
    {
        return OpenArchiveData.OpenResult;
    }

    int RHCode, PFCode;
    RARHeaderDataEx HeaderData;

    HeaderData.CmtBuf = nullptr;
    memset(&OpenArchiveData.Reserved, 0, sizeof(OpenArchiveData.Reserved));

    while ((RHCode = RARReadHeaderEx(hArcData, &HeaderData)) == 0)
    {
        if (wcscmp(fileNameW.c_str(),  HeaderData.FileNameW) == 0)
        {
            std::wstring newFileNameW =
                    QDir::toNativeSeparators(newFileName).toStdWString();
            PFCode = RARProcessFileW(hArcData, RAR_EXTRACT, nullptr,
                                     newFileNameW.c_str());
            returnCode = PFCode;
            break;
        }
        else if ((PFCode = RARProcessFileW(hArcData, RAR_SKIP, nullptr, nullptr))
                 != 0)
        {
            qWarning("%d", PFCode);
            returnCode = PFCode;
            break;
        }
    }

    if (RHCode == ERAR_BAD_DATA)
    {
        qDebug("\nFile header broken");
    }

    RARCloseArchive(hArcData);

    return returnCode;
}

static int CALLBACK CallbackProc(unsigned int msg,
                                 LPARAM myBufferPtr,
                                 LPARAM rarBuffer,
                                 LPARAM bytesProcessed)
{
    switch(msg)
    {
    case UCM_CHANGEVOLUME:
        return -1;

    case UCM_PROCESSDATA:
    {
        char* buf = *reinterpret_cast<char**>(myBufferPtr);
        memcpy(buf, reinterpret_cast<char*>(rarBuffer), static_cast<size_t>(bytesProcessed));
        buf += bytesProcessed;
        return 1;
    }

    case UCM_NEEDPASSWORD:
        return -1;
    default:
        return 0;
    }
}

int ArchiveRar::readFile(const QString &archiveName,
                                  const QString &fileName,
                                  QByteArray &buffer)
{
    const std::wstring fileNameW =
            QDir::toNativeSeparators(fileName).toStdWString();
    const std::wstring arcNameW = archiveName.toStdWString();
    int returnCode = 1000;

    RAROpenArchiveDataEx OpenArchiveData;
    memset(&OpenArchiveData, 0, sizeof(OpenArchiveData));
    OpenArchiveData.ArcNameW = arcNameW.c_str();
    OpenArchiveData.CmtBufSize = 0;
    OpenArchiveData.OpenMode = RAR_OM_EXTRACT;

    Qt::HANDLE hArcData = RAROpenArchiveEx(&OpenArchiveData);

    if (OpenArchiveData.OpenResult != 0)
    {
        return OpenArchiveData.OpenResult;
    }

    char *callBackBuffer = nullptr;
    RARSetCallback(hArcData, CallbackProc, reinterpret_cast<LPARAM>(&callBackBuffer));

    RARHeaderDataEx HeaderData;
    HeaderData.CmtBuf = nullptr;
    memset(&OpenArchiveData.Reserved, 0, sizeof(OpenArchiveData.Reserved));

    int RHCode, PFCode;
    while ((RHCode = RARReadHeaderEx(hArcData, &HeaderData)) == 0)
    {
        if (wcscmp(fileNameW.c_str(), HeaderData.FileNameW) == 0)
        {
            qint64 UnpSize = HeaderData.UnpSize
                    + (static_cast<qint64>(HeaderData.UnpSizeHigh) << 32);
            buffer.resize(UnpSize);
            callBackBuffer = buffer.data();

            PFCode = RARProcessFileW(hArcData, RAR_TEST, nullptr, nullptr);
            returnCode = PFCode;
            break;
        }
        else if ((PFCode = RARProcessFileW(hArcData, RAR_SKIP, nullptr, nullptr))
                 != 0)
        {
            qWarning("%d", PFCode);
            returnCode = PFCode;
            break;
        }
    }

    if (RHCode == ERAR_BAD_DATA)
    {
        qDebug("\nFile header broken");
    }

    RARCloseArchive(hArcData);

    return returnCode;
}

QDateTime ArchiveRar::dateFromDos(const uint32_t dosTime)
{
    const int second = static_cast<int>(dosTime & 0x1f) * 2;
    const int minute = static_cast<int>(dosTime >> 5)  & 0x3f;
    const int hour   = static_cast<int>(dosTime >> 11) & 0x1f;
    const int day    = static_cast<int>(dosTime >> 16) & 0x1f;
    const int month  = static_cast<int>(dosTime >> 21) & 0x0f;
    const int year   = static_cast<int>(dosTime >> 25) + 1980;
    return QDateTime(QDate(year, month, day), QTime(hour, minute, second));
}


ArchiveFileInfo ArchiveRar::createArchiveFileInfo(const RARHeaderDataEx HeaderData)
{
    QString fileName = QString::fromWCharArray(HeaderData.FileNameW);
    fileName = QDir::fromNativeSeparators(fileName);
    quint32 uncompressedSize = 0;
    if ((HeaderData.Flags & RHDF_DIRECTORY) == RHDF_DIRECTORY)
    {
        fileName.append(QDir::separator());
        uncompressedSize = 0;
    }
    else
    {
        qint64 unpSize = HeaderData.UnpSize
                + (static_cast<qint64>(HeaderData.UnpSizeHigh) << 32);
        uncompressedSize = unpSize;
    }
    return ArchiveFileInfo(fileName, dateFromDos(HeaderData.FileTime), uncompressedSize);
}

unsigned int ArchiveRar::getFileInfoList(
        const QString &path,
        std::vector<ArchiveFileInfo> &list)
{
    std::wstring path_wstr = path.toStdWString();

    RAROpenArchiveDataEx OpenArchiveData;
    memset(&OpenArchiveData, 0, sizeof(OpenArchiveData));
    OpenArchiveData.ArcNameW = path_wstr.c_str();
    OpenArchiveData.CmtBufSize = 0;
    OpenArchiveData.OpenMode = RAR_OM_LIST;
    OpenArchiveData.Callback = nullptr;

    Qt::HANDLE hArcData = RAROpenArchiveEx(&OpenArchiveData);

    if (OpenArchiveData.OpenResult != 0)
    {
        return OpenArchiveData.OpenResult;
    }

    int RHCode, PFCode;
    RARHeaderDataEx HeaderData;

    HeaderData.CmtBuf = nullptr;
    memset(&OpenArchiveData.Reserved, 0, sizeof(OpenArchiveData.Reserved));

    while ((RHCode = RARReadHeaderEx(hArcData, &HeaderData)) == 0)
    {
        list.push_back(createArchiveFileInfo(HeaderData));

//#ifdef DEBUG_MODEL_FILES
//        DEBUGOUT << fileName;
//#endif

        if ((PFCode = RARProcessFileW(hArcData, RAR_SKIP, nullptr, nullptr)) != 0)
        {
            qWarning("%d", PFCode);
            break;
        }
    }

    if (RHCode == ERAR_BAD_DATA)
    {
        qDebug("\nFile header broken");
    }

    RARCloseArchive(hArcData);

//#ifdef DEBUG_MODEL_FILES
//    DEBUGOUT << "RAR";
//#endif

    return 0;
}
