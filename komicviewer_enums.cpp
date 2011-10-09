#include "komicviewer_enums.h"

int getArchiveNumberFromTreewidget(int number)
{
    return number - 1100;
}

int makeArchiveNumberForTreewidget(int number)
{
    return number + 1100;
}
