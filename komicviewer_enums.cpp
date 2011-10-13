#include "komicviewer_enums.h"
#include "settings.h"

int getArchiveNumberFromItem(int number)
{
    return number - 1100;
}

int makeArchiveNumberForItem(int number)
{
    return number + 1100;
}

bool isArchive(const QFileInfo &fi)
{
    return Settings::Instance()->getFiltersArchive().contains(fi.suffix().toLower());
}

bool isImage(const QFileInfo &fi)
{
    return Settings::Instance()->getFiltersImage().contains(fi.suffix().toLower());
}

