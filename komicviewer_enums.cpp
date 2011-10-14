#include "komicviewer_enums.h"
#include "settings.h"

bool isArchive(const QFileInfo &fi)
{
    return Settings::Instance()->getFiltersArchive().contains(fi.suffix().toLower());
}

bool isImage(const QFileInfo &fi)
{
    return Settings::Instance()->getFiltersImage().contains(fi.suffix().toLower());
}

