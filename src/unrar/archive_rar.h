#ifndef ARCHIVE_RAR_H
#define ARCHIVE_RAR_H

#include <QLibrary>
#include <QBuffer>

#include "unrar.h"

class ArchiveRar
{
public:
    static bool loadlib();
    static bool extract(const QString &archiveName, const QString &fileName, const QString &newFileName);
    static QByteArray *readFile(const QString &archiveName, const QString &fileName);
    static bool isDir(const RARHeaderDataEx headerData);

private:
    static QLibrary *Lib;
    static unsigned int LHD_DIRECTORY;
    static unsigned int LHD_WINDOWMASK;
};


#endif // ARCHIVE_RAR_H
