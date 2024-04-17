#ifndef ARCHIVEEXTRACTOR_H
#define ARCHIVEEXTRACTOR_H

#include "include/IArchiveExtractor.h"

class ArchiveExtractor : public IArchiveExtractor
{
public:
    explicit ArchiveExtractor();

    virtual std::optional<std::vector<ArchiveFileInfo>> getFileInfoList(
        const QString &path) const override;
    virtual int extract(
        const QString &archiveName,
        const QString &fileName,
        const QString &newFileName) const override;
    virtual std::optional<QByteArray> readFile(
        const QString &archiveName, const QString &fileName, int maxSize) const override;
};

#endif // ARCHIVEEXTRACTOR_H
