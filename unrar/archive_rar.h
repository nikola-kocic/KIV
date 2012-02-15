#ifndef ARCHIVE_RAR_H
#define ARCHIVE_RAR_H

#include <QLibrary>

#include "unrar/unrar.h"

class ArchiveRar
{
public:
    static bool loadlib();

private:
    static QLibrary *Lib;
};


#endif // ARCHIVE_RAR_H
