#include "archive_rar.h"

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
    RARSetChangeVolProc = (RARSetChangeVolProcT)Lib->resolve("RARSetChangeVolProc");
    RARSetProcessDataProc = (RARSetProcessDataProcT)Lib->resolve("RARSetProcessDataProc");
    RARSetPassword = (RARSetPasswordT)Lib->resolve("RARSetPassword");
    RARGetDllVersion = (RARGetDllVersionT)Lib->resolve("RARGetDllVersion");

    return true;
}
