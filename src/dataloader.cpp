#include "dataloader.h"

//#define DEBUG_DATA_LOADER
#ifdef DEBUG_DATA_LOADER
#include "helper.h"
#endif

DataLoader::DataLoader(const IArchiveExtractor* const archive_extractor)
    : m_archive_extractor(archive_extractor)
{
}

QByteArray DataLoader::getData(const FileInfo &file_info, int maxSize) const
{
    QByteArray data;
#ifdef DEBUG_DATA_LOADER
    DEBUGOUT << file_info.getPath();
#endif
    if (file_info.fileExists())
    {
        if (file_info.isInArchive())
        {
            data = getArchiveFileData(file_info, maxSize);
        }
        else
        {
            data = getFileData(file_info, maxSize);
        }
    }
    return data;
}

QByteArray DataLoader::getFileData(const FileInfo &file_info, int maxSize) const
{
    QFile file(file_info.getPath());
#ifdef DEBUG_DATA_LOADER
    DEBUGOUT << file_info.getPath();
#endif
    file.open(QIODevice::ReadOnly);
    QByteArray data;
    if (maxSize == -1)
    {
        data = file.readAll();
    }
    else
    {
        data = file.read(maxSize);
    }
    file.close();
    return data;
}


QByteArray DataLoader::getArchiveFileData(const FileInfo &file_info, int maxSize) const
{
    QByteArray buff;
    int success = m_archive_extractor->readFile(
                file_info.getContainerPath(),
                file_info.getArchiveImagePath(),
                buff,
                maxSize);

    if (success != 0 || buff.isEmpty())
    {
        return QByteArray();
    }

#ifdef DEBUG_DATA_LOADER
    DEBUGOUT << "finished reading from archive"
             << file_info.getPath();
#endif
    return buff;
}
