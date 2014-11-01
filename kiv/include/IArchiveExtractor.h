#ifndef IARCHIVEEXTRACTOR_H
#define IARCHIVEEXTRACTOR_H

#include "kiv/src/enums.h"

class IArchiveExtractor
{
public:
    // Empty virtual destructor for proper cleanup
    virtual ~IArchiveExtractor() {}

    virtual int getFileInfoList(const QString &path,
                                QList<const ArchiveFileInfo*> &list) const = 0;
    virtual int extract(const QString &archiveName,
                        const QString &fileName,
                        const QString &newFileName) const = 0;
    virtual int readFile(const QString &archiveName,
                         const QString &fileName,
                         QByteArray &buffer) const = 0;
};

#endif // IARCHIVEEXTRACTOR_H
