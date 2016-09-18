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
    QByteArray getData(const FileInfo &file_info) const;

protected:
    QByteArray getArchiveFileData(const FileInfo &file_info) const;
    QByteArray getFileData(const FileInfo &file_info) const;

protected:
    const IArchiveExtractor* const m_archive_extractor;
};

#endif // DATALOADER_H
