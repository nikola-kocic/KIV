#include "archive_rar.h"
#include <QDir>
#include <QBuffer>

QLibrary *ArchiveRar::Lib = 0;
unsigned int ArchiveRar::LHD_DIRECTORY = 0x00e0U;
unsigned int ArchiveRar::LHD_WINDOWMASK = 0x00e0U;

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
    RARSetChangeVolProc = (RARSetChangeVolProcT)Lib->resolve("RARSetChangeVolProc");
    RARSetProcessDataProc = (RARSetProcessDataProcT)Lib->resolve("RARSetProcessDataProc");
    RARSetPassword = (RARSetPasswordT)Lib->resolve("RARSetPassword");
    RARGetDllVersion = (RARGetDllVersionT)Lib->resolve("RARGetDllVersion");

    return true;
}

bool ArchiveRar::extract(const QString &archiveName, const QString &fileName, const QString &newFileName)
{
    bool success = false;
    std::wstring arcNameW = archiveName.toStdWString();

    struct RAROpenArchiveDataEx OpenArchiveData;
    memset(&OpenArchiveData, 0, sizeof(OpenArchiveData));
    OpenArchiveData.ArcNameW = arcNameW.c_str();
    OpenArchiveData.CmtBufSize = 0;
    OpenArchiveData.OpenMode = RAR_OM_EXTRACT;
    OpenArchiveData.Callback = 0;
    OpenArchiveData.UserData = 0;

    Qt::HANDLE hArcData = RAROpenArchiveEx(&OpenArchiveData);

    if (OpenArchiveData.OpenResult == 0)
    {
        int RHCode, PFCode;
        struct RARHeaderDataEx HeaderData;

        HeaderData.CmtBuf = NULL;
        memset(&OpenArchiveData.Reserved, 0, sizeof(OpenArchiveData.Reserved));

        while ((RHCode = RARReadHeaderEx(hArcData, &HeaderData)) == 0)
        {
            const QString currentFileName = QString::fromWCharArray(HeaderData.FileNameW);
            if (currentFileName == fileName)
            {
                std::wstring newFileNameW = QDir::toNativeSeparators(newFileName).toStdWString();
                RARProcessFileW(hArcData, RAR_EXTRACT, NULL, newFileNameW.c_str());
                success = true;
                break;
            }
            else
            {
                if ((PFCode = RARProcessFileW(hArcData, RAR_SKIP, NULL, NULL)) != 0)
                {
                    qWarning("%d", PFCode);
                    break;
                }
            }
        }

        if (RHCode == ERAR_BAD_DATA)
        {
            qDebug("\nFile header broken");
        }

        RARCloseArchive(hArcData);
    }

    return success;
}

static int CALLBACK CallbackProc(unsigned int msg, LPARAM myBufferPtr, LPARAM rarBuffer, LPARAM bytesProcessed)
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

QByteArray *ArchiveRar::readFile(const QString &archiveName, const QString &fileName)
{
    QByteArray *out = 0;
    std::wstring arcNameW = archiveName.toStdWString();

    struct RAROpenArchiveDataEx OpenArchiveData;
    memset(&OpenArchiveData, 0, sizeof(OpenArchiveData));
    OpenArchiveData.ArcNameW = arcNameW.c_str();
    OpenArchiveData.CmtBufSize = 0;
    OpenArchiveData.OpenMode = RAR_OM_EXTRACT;

    Qt::HANDLE hArcData = RAROpenArchiveEx(&OpenArchiveData);

    if (OpenArchiveData.OpenResult == 0)
    {
        char *callBackBuffer = 0;
        RARSetCallback(hArcData, CallbackProc, (LPARAM)&callBackBuffer);

        struct RARHeaderDataEx HeaderData;
        HeaderData.CmtBuf = NULL;
        memset(&OpenArchiveData.Reserved, 0, sizeof(OpenArchiveData.Reserved));

        int RHCode, PFCode;
        while ((RHCode = RARReadHeaderEx(hArcData, &HeaderData)) == 0)
        {
            const QString currentFileName = QString::fromWCharArray(HeaderData.FileNameW);
            if (currentFileName == fileName)
            {
                qint64 UnpSize = HeaderData.UnpSize + (((qint64)HeaderData.UnpSizeHigh) << 32);
                char *buffer = new char[UnpSize];
                callBackBuffer = buffer;

                PFCode = RARProcessFileW(hArcData, RAR_TEST, NULL, NULL);
                callBackBuffer = 0;
                out = new QByteArray(buffer, UnpSize);
                delete[] buffer;

                break;
            }
            else
            {
                if ((PFCode = RARProcessFileW(hArcData, RAR_SKIP, NULL, NULL)) != 0)
                {
                    qWarning("%d", PFCode);
                    break;
                }
            }
        }

        if (RHCode == ERAR_BAD_DATA)
        {
            qDebug("\nFile header broken");
        }

        RARCloseArchive(hArcData);
    }

    if (out == 0)
    {
        return new QByteArray();
    }
    return out;
}

bool ArchiveRar::isDir(const RARHeaderDataEx &headerData)
{
    return((headerData.Flags & LHD_WINDOWMASK) == LHD_DIRECTORY);
}
