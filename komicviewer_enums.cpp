#include "komicviewer_enums.h"
#include "settings.h"

bool checkFileExtension(const QFileInfo &fi)
{
    if (isArchive(fi) || fi.isDir())
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
    return Settings::Instance()->getFiltersArchive().contains(fi.suffix().toLower());
}

bool isImage(const QFileInfo &fi)
{
    return Settings::Instance()->getFiltersImage().contains(fi.suffix().toLower());
}

