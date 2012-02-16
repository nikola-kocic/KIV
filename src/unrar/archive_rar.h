#ifndef ARCHIVE_RAR_H
#define ARCHIVE_RAR_H

#include <QLibrary>
#include <QBuffer>

#include "unrar/unrar.h"

class ArchiveRar
{
public:
    static bool loadlib();
    static bool extract(const QString &archiveName, const QString &fileName, const QString &newFileName);
    static QByteArray *readFile(const QString &archiveName, const QString &fileName);

private:
    static QLibrary *Lib;
};


#endif // ARCHIVE_RAR_H
