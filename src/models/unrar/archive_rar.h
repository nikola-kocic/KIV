#ifndef ARCHIVE_RAR_H
#define ARCHIVE_RAR_H

#include <vector>
#include <QBuffer>
#include <QLibrary>

#include <optional>
#include <vector>

#include "enums.h"

struct RARHeaderDataEx;

class ArchiveRar
{
public:
    static bool loadlib();
    static bool extract(
        const QString &archiveName, const QString &fileName, const QString &newFileName);
    static std::optional<QByteArray> readFile(
        const QString &archiveName, const QString &fileName, int maxSize);
    static std::optional<std::vector<ArchiveFileInfo>> getFileInfoList(const QString &path);
    static ArchiveFileInfo createArchiveFileInfo(const RARHeaderDataEx HeaderData);

private:
    static QDateTime dateFromDos(const uint32_t dosTime);
    static QLibrary *Lib;
};

#endif // ARCHIVE_RAR_H
