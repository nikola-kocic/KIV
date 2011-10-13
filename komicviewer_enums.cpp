#include "komicviewer_enums.h"

int getArchiveNumberFromItem(int number)
{
    return number - 1100;
}

int makeArchiveNumberForItem(int number)
{
    return number + 1100;
}
