#ifndef ARCHIVE_RAR_H
#define ARCHIVE_RAR_H

#include <QLibrary>
#include <QBuffer>

#include "kiv/src/enums.h"

struct RARHeaderDataEx;

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
                                        QList<const ArchiveFileInfo *> &list);
    static const ArchiveFileInfo *createArchiveFileInfo(const RARHeaderDataEx HeaderData);

private:
    static QDateTime dateFromDos(const uint dosTime);
    static QLibrary *Lib;
};


#endif  // ARCHIVE_RAR_H
