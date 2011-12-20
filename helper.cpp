#include "helper.h"
#include "settings.h"

//#include <QDebug>

bool checkFileExtension(const QFileInfo &info)
{
    if (isArchiveFile(info) || info.isDir() || isImageFile(info))
    {
       return true;
    }
    else
    {
        return false;
    }
}

bool isArchiveFile(const QFileInfo &fi)
{
    return FileInfo::getFiltersArchive().contains(fi.suffix().toLower());
}

bool isImageFile(const QFileInfo &fi)
{
    return FileInfo::getFiltersImage().contains(fi.suffix().toLower());
}
