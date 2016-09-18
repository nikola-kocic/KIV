#ifndef ARCHIVE_RAR_H
#define ARCHIVE_RAR_H

#include <memory>
#include <vector>
#include <QLibrary>
#include <QBuffer>

#include "enums.h"

struct RARHeaderDataEx;

class ArchiveRar
{
public:
    static bool loadlib();
    static bool extract(
            const QString &archiveName,
            const QString &fileName,
            const QString &newFileName);
    static bool readFile(
            const QString &archiveName,
            const QString &fileName,
            QByteArray &buffer,
            int maxSize);
    static unsigned int getFileInfoList(const QString &path,
                                        std::vector<ArchiveFileInfo> &list);
    static ArchiveFileInfo createArchiveFileInfo(const RARHeaderDataEx HeaderData);

private:
    static QDateTime dateFromDos(const uint32_t dosTime);
    static QLibrary *Lib;
};


#endif  // ARCHIVE_RAR_H
