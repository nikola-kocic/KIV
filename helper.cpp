#include "helper.h"
#include "settings.h"

//#include <QDebug>

bool checkFileExtension(const QFileInfo &info)
{
    if (isArchive(info) || info.isDir() || isImage(info))
    {
       return true;
    }
    else
    {
        return false;
    }
}

bool isArchive(const QFileInfo &fi)
{
    return FileInfo::getFiltersArchive().contains(fi.suffix().toLower());
}

bool isImage(const QFileInfo &fi)
{
    return FileInfo::getFiltersImage().contains(fi.suffix().toLower());
}
