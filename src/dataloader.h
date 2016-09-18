#ifndef DATALOADER_H
#define DATALOADER_H

#include <QByteArray>

#include <memory>

#include "fileinfo.h"
#include "include/IArchiveExtractor.h"

class DataLoader
{
public:
    DataLoader(std::unique_ptr<const IArchiveExtractor> archive_extractor);
    QByteArray getData(const FileInfo &file_info) const;

protected:
    QByteArray getArchiveFileData(const FileInfo &file_info) const;
    QByteArray getFileData(const FileInfo &file_info) const;

protected:
    std::unique_ptr<const IArchiveExtractor> m_archive_extractor;
};

#endif // DATALOADER_H
