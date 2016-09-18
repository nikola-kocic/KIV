#ifndef ARCHIVEEXTRACTOR_H
#define ARCHIVEEXTRACTOR_H

#include "include/IArchiveExtractor.h"

class ArchiveExtractor: public IArchiveExtractor
{
public:
    explicit ArchiveExtractor();

    virtual int getFileInfoList(const QString &path,
                                std::vector<ArchiveFileInfo> &list) const override;
    virtual int extract(const QString &archiveName,
                        const QString &fileName,
                        const QString &newFileName) const override;
    virtual int readFile(const QString &archiveName,
                         const QString &fileName,
                         QByteArray &buffer) const override;
};

#endif // ARCHIVEEXTRACTOR_H
