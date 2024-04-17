#include "models/unrar/archive_rar.h"

#include <QBuffer>
#include <QDir>

#include <limits>

#include "models/unrar/unrar.h"

#define LHD_LARGE 0x0100
#define LHD_UNICODE 0x0200
#define LHD_SALT 0x0400
#define LHD_EXTTIME 0x1000
#define LHD_WINDOWMASK 0x00e0
#define LHD_DIRECTORY 0x00e0
#define LONG_BLOCK 0x8000

RAROpenArchiveExT RAROpenArchiveEx = nullptr;
RARCloseArchiveT RARCloseArchive = nullptr;
RARReadHeaderExT RARReadHeaderEx = nullptr;
RARProcessFileWT RARProcessFileW = nullptr;
RARSetCallbackT RARSetCallback = nullptr;
RARSetChangeVolProcT RARSetChangeVolProc = nullptr;
RARSetProcessDataProcT RARSetProcessDataProc = nullptr;
RARSetPasswordT RARSetPassword = nullptr;
RARGetDllVersionT RARGetDllVersion = nullptr;

struct RAROpenArchiveDataEx;

QLibrary *ArchiveRar::Lib = nullptr;

bool ArchiveRar::loadlib()
{
    if (Lib) {
        return true;
    }

    Lib = new QLibrary("unrar");
    if (!Lib->load()) {
        Lib = nullptr;
        return false;
    }

    RAROpenArchiveEx = reinterpret_cast<RAROpenArchiveExT>(Lib->resolve("RAROpenArchiveEx"));
    Q_ASSERT(RAROpenArchiveEx);
    RARCloseArchive = reinterpret_cast<RARCloseArchiveT>(Lib->resolve("RARCloseArchive"));
    Q_ASSERT(RARCloseArchive);
    RARReadHeaderEx = reinterpret_cast<RARReadHeaderExT>(Lib->resolve("RARReadHeaderEx"));
    Q_ASSERT(RARReadHeaderEx);
    RARProcessFileW = reinterpret_cast<RARProcessFileWT>(Lib->resolve("RARProcessFileW"));
    Q_ASSERT(RARProcessFileW);
    RARSetCallback = reinterpret_cast<RARSetCallbackT>(Lib->resolve("RARSetCallback"));
    Q_ASSERT(RARSetCallback);
    RARSetChangeVolProc = reinterpret_cast<RARSetChangeVolProcT>(
        Lib->resolve("RARSetChangeVolProc"));
    Q_ASSERT(RARSetChangeVolProc);
    RARSetProcessDataProc = reinterpret_cast<RARSetProcessDataProcT>(
        Lib->resolve("RARSetProcessDataProc"));
    Q_ASSERT(RARSetProcessDataProc);
    RARSetPassword = reinterpret_cast<RARSetPasswordT>(Lib->resolve("RARSetPassword"));
    Q_ASSERT(RARSetPassword);
    RARGetDllVersion = reinterpret_cast<RARGetDllVersionT>(Lib->resolve("RARGetDllVersion"));
    Q_ASSERT(RARGetDllVersion);

    return true;
}

static qint64 getUncompressedSize(const RARHeaderDataEx &HeaderData)
{
    const qint64 uncompressedSize = HeaderData.UnpSize
                                    + (static_cast<qint64>(HeaderData.UnpSizeHigh) << 32);
    return uncompressedSize;
}

bool ArchiveRar::extract(
    const QString &archiveName, const QString &fileName, const QString &newFileName)
{
    const std::wstring fileNameW = QDir::toNativeSeparators(fileName).toStdWString();
    const std::wstring arcNameW = archiveName.toStdWString();
    bool retval = true;

    RAROpenArchiveDataEx OpenArchiveData;
    memset(&OpenArchiveData, 0, sizeof(OpenArchiveData));
    OpenArchiveData.ArcNameW = arcNameW.c_str();
    OpenArchiveData.OpenMode = RAR_OM_EXTRACT;

    const Qt::HANDLE hArcData = RAROpenArchiveEx(&OpenArchiveData);

    if (OpenArchiveData.OpenResult != 0) {
        qWarning("RAROpenArchiveEx error: %d", OpenArchiveData.OpenResult);
        return false;
    }

    int RHCode, PFCode;
    RARHeaderDataEx HeaderData;

    HeaderData.CmtBuf = nullptr;
    memset(&OpenArchiveData.Reserved, 0, sizeof(OpenArchiveData.Reserved));

    while ((RHCode = RARReadHeaderEx(hArcData, &HeaderData)) == 0) {
        if (wcscmp(fileNameW.c_str(), HeaderData.FileNameW) == 0) {
            const std::wstring newFileNameW = QDir::toNativeSeparators(newFileName).toStdWString();

            if ((PFCode = RARProcessFileW(hArcData, RAR_EXTRACT, nullptr, newFileNameW.c_str()))
                != 0) {
                qWarning("RARProcessFileW RAR_EXTRACT error: %d", PFCode);
                retval = false;
            }
            break;
        } else if ((PFCode = RARProcessFileW(hArcData, RAR_SKIP, nullptr, nullptr)) != 0) {
            qWarning("RARProcessFileW RAR_SKIP error: %d", PFCode);
            retval = false;
            break;
        }
    }

    if (RHCode == ERAR_BAD_DATA) {
        qDebug("\nFile header broken");
    }

    RARCloseArchive(hArcData);

    return retval;
}

struct CallbackData
{
    CallbackData(QByteArray &_buf, int _maxSize)
        : buf(_buf)
        , maxSize(_maxSize)
    {}
    QByteArray &buf;
    int maxSize;
};

static int CALLBACK
CallbackProc(unsigned int msg, LPARAM UserData, LPARAM unpackedDataParam, LPARAM unpackedDataSize)
{
    switch (msg) {
    case UCM_CHANGEVOLUME:
        return -1;

    case UCM_PROCESSDATA: {
        auto *data = reinterpret_cast<CallbackData *>(UserData);
        Q_ASSERT(data != nullptr);
        char *unpackedData = reinterpret_cast<char *>(unpackedDataParam);
        Q_ASSERT(unpackedData != nullptr);
        data->buf.append(unpackedData, static_cast<int>(unpackedDataSize));
        if (data->maxSize != -1 && data->buf.size() >= data->maxSize) {
            return -1;
        }
        return 1;
    }

    case UCM_NEEDPASSWORD:
        return -1;
    default:
        return 0;
    }
}

std::optional<QByteArray> ArchiveRar::readFile(
    const QString &archiveName, const QString &fileName, int maxSize)
{
    const std::wstring fileNameW = QDir::toNativeSeparators(fileName).toStdWString();
    const std::wstring arcNameW = archiveName.toStdWString();

    RAROpenArchiveDataEx openArchiveData{};
    openArchiveData.ArcNameW = arcNameW.c_str();
    openArchiveData.OpenMode = RAR_OM_EXTRACT;

    Qt::HANDLE hArcData = RAROpenArchiveEx(&openArchiveData);

    if (openArchiveData.OpenResult != 0) {
        qWarning() << "RAROpenArchiveEx error: " << openArchiveData.OpenResult;
        return std::nullopt;
    }

    auto buffer = QByteArray{};
    auto callBackUserData = CallbackData(buffer, maxSize);
    RARSetCallback(hArcData, CallbackProc, reinterpret_cast<LPARAM>(&callBackUserData));

    int rh_code, pf_code;
    do {
        RARHeaderDataEx headerData{};
        rh_code = RARReadHeaderEx(hArcData, &headerData);
        if (wcscmp(fileNameW.c_str(), headerData.FileNameW) == 0) {
            if (maxSize == -1) {
                qint64 unpSize = getUncompressedSize(headerData);
                if (unpSize > (std::numeric_limits<int>::max)()) {
                    qWarning("File size too large: %lld", unpSize);
                    buffer.clear();
                    break;
                }
                buffer.reserve(static_cast<int>(unpSize));
            }

            if ((pf_code = RARProcessFileW(hArcData, RAR_TEST, nullptr, nullptr) != 0)) {
                qWarning("RARProcessFileW RAR_TEST error: %d", pf_code);
                buffer.clear();
                break;
            }
        } else if ((pf_code = RARProcessFileW(hArcData, RAR_SKIP, nullptr, nullptr)) != 0) {
            qWarning("RARProcessFileW RAR_SKIP error: %d", pf_code);
            buffer.clear();
            break;
        }
    } while (rh_code == 0);

    if (rh_code == ERAR_BAD_DATA) {
        qDebug("\nFile header broken");
    }

    RARCloseArchive(hArcData);

    if (buffer.isEmpty()) {
        return std::nullopt;
    }
    return buffer;
}

QDateTime ArchiveRar::dateFromDos(const uint32_t dosTime)
{
    const int second = static_cast<int>(dosTime & 0x1f) * 2;
    const int minute = static_cast<int>(dosTime >> 5) & 0x3f;
    const int hour = static_cast<int>(dosTime >> 11) & 0x1f;
    const int day = static_cast<int>(dosTime >> 16) & 0x1f;
    const int month = static_cast<int>(dosTime >> 21) & 0x0f;
    const int year = static_cast<int>(dosTime >> 25) + 1980;
    return QDateTime(QDate(year, month, day), QTime(hour, minute, second));
}

ArchiveFileInfo ArchiveRar::createArchiveFileInfo(const RARHeaderDataEx headerData)
{
    QString fileName = QString::fromWCharArray(headerData.FileNameW);
    fileName = QDir::fromNativeSeparators(fileName);
    qint64 uncompressedSize = 0;
    if ((headerData.Flags & RHDF_DIRECTORY) == RHDF_DIRECTORY) {
        fileName.append(QDir::separator());
        uncompressedSize = 0;
    } else {
        uncompressedSize = getUncompressedSize(headerData);
    }
    return ArchiveFileInfo(fileName, dateFromDos(headerData.FileTime), uncompressedSize);
}

std::optional<std::vector<ArchiveFileInfo>> ArchiveRar::getFileInfoList(const QString &path)
{
    std::wstring path_wstr = path.toStdWString();

    RAROpenArchiveDataEx openArchiveData{};
    openArchiveData.ArcNameW = path_wstr.c_str();
    openArchiveData.OpenMode = RAR_OM_LIST;

    Qt::HANDLE hArcData = RAROpenArchiveEx(&openArchiveData);

    if (openArchiveData.OpenResult != 0) {
        return std::nullopt;
    }

    auto list = std::vector<ArchiveFileInfo>{};
    int rh_code, pf_code;
    do {
        RARHeaderDataEx headerData{};
        rh_code = RARReadHeaderEx(hArcData, &headerData);
        list.push_back(createArchiveFileInfo(headerData));

        //#ifdef DEBUG_MODEL_FILES
        //        DEBUGOUT << fileName;
        //#endif

        if ((pf_code = RARProcessFileW(hArcData, RAR_SKIP, nullptr, nullptr)) != 0) {
            qWarning() << "RARProcessFileW RAR_SKIP error " << pf_code;
            break;
        }
    } while (rh_code == 0);

    if (rh_code == ERAR_BAD_DATA) {
        qDebug("\nFile header broken");
    }

    RARCloseArchive(hArcData);

    //#ifdef DEBUG_MODEL_FILES
    //    DEBUGOUT << "RAR";
    //#endif

    return list;
}
