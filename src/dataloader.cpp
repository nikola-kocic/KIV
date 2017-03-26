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
        Identifiers<QString> identifiers = file_info.getIdentifiers();
        if (identifiers.childIdentifier.isEmpty())
        {
            data = getFileData(identifiers.parentIdentifier, maxSize);
        }
        else
        {
            data = getArchiveFileData(identifiers.parentIdentifier, identifiers.childIdentifier, maxSize);
        }
    }
    return data;
}

QByteArray DataLoader::getFileData(const QString &file_path, int maxSize) const
{
    QFile file(file_path);
#ifdef DEBUG_DATA_LOADER
    DEBUGOUT << file_info.getPath();
#endif
    QByteArray data;
    if (!file.open(QIODevice::ReadOnly))
    {
        return data;
    }
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


QByteArray DataLoader::getArchiveFileData(
        const QString &archive_path, const QString &archive_image_path, int maxSize) const
{
    QByteArray buff;
    int success = m_archive_extractor->readFile(
                archive_path,
                archive_image_path,
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
