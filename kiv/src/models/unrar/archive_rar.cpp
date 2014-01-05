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

RAROpenArchiveT        RAROpenArchive = 0;
RAROpenArchiveExT      RAROpenArchiveEx = 0;
RARCloseArchiveT       RARCloseArchive = 0;
RARReadHeaderT         RARReadHeader = 0;
RARReadHeaderExT       RARReadHeaderEx = 0;
RARProcessFileT        RARProcessFile = 0;
RARProcessFileWT       RARProcessFileW = 0;
RARSetCallbackT        RARSetCallback = 0;
RARSetChangeVolProcT   RARSetChangeVolProc = 0;
RARSetProcessDataProcT RARSetProcessDataProc = 0;
RARSetPasswordT        RARSetPassword = 0;
RARGetDllVersionT      RARGetDllVersion = 0;

QLibrary *ArchiveRar::Lib = 0;

bool ArchiveRar::loadlib()
{
    if (Lib)
    {
        return true;
    }

    Lib = new QLibrary("unrar");
    if (!Lib->load())
    {
        Lib = 0;
        return false;
    }

    RAROpenArchive = (RAROpenArchiveT)Lib->resolve("RAROpenArchive");
    RAROpenArchiveEx = (RAROpenArchiveExT)Lib->resolve("RAROpenArchiveEx");
    RARCloseArchive = (RARCloseArchiveT)Lib->resolve("RARCloseArchive");
    RARReadHeader = (RARReadHeaderT)Lib->resolve("RARReadHeader");
    RARReadHeaderEx = (RARReadHeaderExT)Lib->resolve("RARReadHeaderEx");
    RARProcessFile = (RARProcessFileT)Lib->resolve("RARProcessFile");
    RARProcessFileW = (RARProcessFileWT)Lib->resolve("RARProcessFileW");
    RARSetCallback = (RARSetCallbackT)Lib->resolve("RARSetCallback");
    RARSetChangeVolProc =
            (RARSetChangeVolProcT)Lib->resolve("RARSetChangeVolProc");
    RARSetProcessDataProc =
            (RARSetProcessDataProcT)Lib->resolve("RARSetProcessDataProc");
    RARSetPassword = (RARSetPasswordT)Lib->resolve("RARSetPassword");
    RARGetDllVersion = (RARGetDllVersionT)Lib->resolve("RARGetDllVersion");

    return true;
}

unsigned int ArchiveRar::extract(const QString &archiveName,
                                 const QString &fileName,
                                 const QString &newFileName)
{
    const std::wstring fileNameW =
            QDir::toNativeSeparators(fileName).toStdWString();
    const std::wstring arcNameW = archiveName.toStdWString();
    unsigned int returnCode = 1000;

    struct RAROpenArchiveDataEx OpenArchiveData;
    memset(&OpenArchiveData, 0, sizeof(OpenArchiveData));
    OpenArchiveData.ArcNameW = arcNameW.c_str();
    OpenArchiveData.CmtBufSize = 0;
    OpenArchiveData.OpenMode = RAR_OM_EXTRACT;
    OpenArchiveData.Callback = 0;
    OpenArchiveData.UserData = 0;

    Qt::HANDLE hArcData = RAROpenArchiveEx(&OpenArchiveData);

    if (OpenArchiveData.OpenResult != 0)
    {
        return OpenArchiveData.OpenResult;
    }

    int RHCode, PFCode;
    struct RARHeaderDataEx HeaderData;

    HeaderData.CmtBuf = NULL;
    memset(&OpenArchiveData.Reserved, 0, sizeof(OpenArchiveData.Reserved));

    while ((RHCode = RARReadHeaderEx(hArcData, &HeaderData)) == 0)
    {
        if (wcscmp(fileNameW.c_str(),  HeaderData.FileNameW) == 0)
        {
            std::wstring newFileNameW =
                    QDir::toNativeSeparators(newFileName).toStdWString();
            PFCode = RARProcessFileW(hArcData, RAR_EXTRACT, NULL,
                                     newFileNameW.c_str());
            returnCode = PFCode;
            break;
        }
        else if ((PFCode = RARProcessFileW(hArcData, RAR_SKIP, NULL, NULL))
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
        memcpy(*(char**)myBufferPtr, (char*)rarBuffer, bytesProcessed);
        *(char**)myBufferPtr += bytesProcessed;
        return 1;

    case UCM_NEEDPASSWORD:
        return -1;
    }

    return 0;
}

unsigned int ArchiveRar::readFile(const QString &archiveName,
                                  const QString &fileName,
                                  QByteArray &buffer)
{
    const std::wstring fileNameW =
            QDir::toNativeSeparators(fileName).toStdWString();
    const std::wstring arcNameW = archiveName.toStdWString();
    unsigned int returnCode = 1000;

    struct RAROpenArchiveDataEx OpenArchiveData;
    memset(&OpenArchiveData, 0, sizeof(OpenArchiveData));
    OpenArchiveData.ArcNameW = arcNameW.c_str();
    OpenArchiveData.CmtBufSize = 0;
    OpenArchiveData.OpenMode = RAR_OM_EXTRACT;

    Qt::HANDLE hArcData = RAROpenArchiveEx(&OpenArchiveData);

    if (OpenArchiveData.OpenResult != 0)
    {
        return OpenArchiveData.OpenResult;
    }

    char *callBackBuffer = 0;
    RARSetCallback(hArcData, CallbackProc, (LPARAM)&callBackBuffer);

    struct RARHeaderDataEx HeaderData;
    HeaderData.CmtBuf = NULL;
    memset(&OpenArchiveData.Reserved, 0, sizeof(OpenArchiveData.Reserved));

    int RHCode, PFCode;
    while ((RHCode = RARReadHeaderEx(hArcData, &HeaderData)) == 0)
    {
        if (wcscmp(fileNameW.c_str(), HeaderData.FileNameW) == 0)
        {
            qint64 UnpSize = HeaderData.UnpSize
                    + (((qint64)HeaderData.UnpSizeHigh) << 32);
            buffer.resize(UnpSize);
            callBackBuffer = buffer.data();

            PFCode = RARProcessFileW(hArcData, RAR_TEST, NULL, NULL);
            returnCode = PFCode;
            break;
        }
        else if ((PFCode = RARProcessFileW(hArcData, RAR_SKIP, NULL, NULL))
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

QDateTime ArchiveRar::dateFromDos(const uint dosTime)
{
    const uint second = (dosTime & 0x1f) * 2;
    const uint minute = (dosTime >> 5)  & 0x3f;
    const uint hour   = (dosTime >> 11) & 0x1f;
    const uint day    = (dosTime >> 16) & 0x1f;
    const uint month  = (dosTime >> 21) & 0x0f;
    const uint year   = (dosTime >> 25) + 1980;
    return QDateTime(QDate(year, month, day), QTime(hour, minute, second));
}

unsigned int ArchiveRar::getFileInfoList(const QString &path,
                                         QList<ArchiveFileInfo> &list)
{
    std::wstring path_wstr = path.toStdWString();

    struct RAROpenArchiveDataEx OpenArchiveData;
    memset(&OpenArchiveData, 0, sizeof(OpenArchiveData));
    OpenArchiveData.ArcNameW = path_wstr.c_str();
    OpenArchiveData.CmtBufSize = 0;
    OpenArchiveData.OpenMode = RAR_OM_LIST;
    OpenArchiveData.Callback = NULL;

    Qt::HANDLE hArcData = RAROpenArchiveEx(&OpenArchiveData);

    if (OpenArchiveData.OpenResult != 0)
    {
        return OpenArchiveData.OpenResult;
    }

    int RHCode, PFCode;
    struct RARHeaderDataEx HeaderData;

    HeaderData.CmtBuf = NULL;
    memset(&OpenArchiveData.Reserved, 0, sizeof(OpenArchiveData.Reserved));

    while ((RHCode = RARReadHeaderEx(hArcData, &HeaderData)) == 0)
    {
        ArchiveFileInfo newArchiveFileInfo;
        QString fileName = QString::fromWCharArray(HeaderData.FileNameW);
        fileName = QDir::fromNativeSeparators(fileName);
        if ((HeaderData.Flags & LHD_WINDOWMASK) == LHD_DIRECTORY)
        {
            fileName.append(QDir::separator());
            newArchiveFileInfo.uncompressedSize = 0;
        }
        else
        {
            qint64 unpSize = HeaderData.UnpSize
                    + (((qint64)HeaderData.UnpSizeHigh) << 32);
            newArchiveFileInfo.uncompressedSize = unpSize;
        }
        newArchiveFileInfo.name = fileName;
        newArchiveFileInfo.dateTime = dateFromDos(HeaderData.FileTime);

        list.append(newArchiveFileInfo);

//#ifdef DEBUG_MODEL_FILES
//        DEBUGOUT << fileName;
//#endif

        if ((PFCode = RARProcessFileW(hArcData, RAR_SKIP, NULL, NULL)) != 0)
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
