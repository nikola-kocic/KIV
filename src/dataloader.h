#ifndef DATALOADER_H
#define DATALOADER_H

#include <QByteArray>

#include <memory>

#include "fileinfo.h"
#include "include/IArchiveExtractor.h"

class DataLoader
{
public:
    DataLoader(const IArchiveExtractor* const archive_extractor);
    QByteArray getData(const FileInfo &file_info, int maxSize) const;

protected:
    QByteArray getArchiveFileData(const QString &archive_path, const QString &archive_image_path, int maxSize) const;
    QByteArray getFileData(const QString &file_path, int maxSize) const;

protected:
    const IArchiveExtractor* const m_archive_extractor;
};

#endif // DATALOADER_H
