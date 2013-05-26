#ifndef ARCHIVE_RAR_H
#define ARCHIVE_RAR_H

#include <QLibrary>
#include <QBuffer>

#include "enums.h"

class ArchiveRar
{
public:
    static bool loadlib();
    static unsigned int extract(const QString &archiveName,
                                const QString &fileName,
                                const QString &newFileName);
    static unsigned int readFile(const QString &archiveName,
                                 const QString &fileName,
                                 QByteArray &buffer);
    static unsigned int getFileInfoList(const QString &path,
                                        QList<ArchiveFileInfo> &list);

private:
    static QDateTime dateFromDos(const uint dosTime);
    static QLibrary *Lib;
};


#endif  // ARCHIVE_RAR_H
