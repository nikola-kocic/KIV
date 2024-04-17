#ifndef IARCHIVEEXTRACTOR_H
#define IARCHIVEEXTRACTOR_H

#include "enums.h"
#include <optional>
#include <vector>

class IArchiveExtractor
{
public:
    // Empty virtual destructor for proper cleanup
    virtual ~IArchiveExtractor();

    virtual std::optional<std::vector<ArchiveFileInfo>> getFileInfoList(const QString &path) const
        = 0;
    virtual int extract(
        const QString &archiveName, const QString &fileName, const QString &newFileName) const
        = 0;
    virtual std::optional<QByteArray> readFile(
        const QString &archiveName, const QString &fileName, int maxSize) const
        = 0;
};

#endif // IARCHIVEEXTRACTOR_H
