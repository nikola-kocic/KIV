#include "archiveextractor.h"

#include <QFile>

#include <quazip/JlCompress.h>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include "kiv/src/models/unrar/archive_rar.h"


ArchiveExtractor::ArchiveExtractor(QObject *parent) :
    QObject(parent)
{
}

int ArchiveExtractor::getFileInfoList(const QString &path,
                                      std::vector<std::unique_ptr<const ArchiveFileInfo> > &list) const
{
    QFile archiveFile(path);

    // Try to open as ZIP
    QuaZip zip(&archiveFile);
    if (zip.open(QuaZip::mdUnzip))
    {
        zip.setFileNameCodec("UTF-8");
        QList<QuaZipFileInfo> zipList = zip.getFileInfoList();
        while(!zipList.empty())
        {
            const QuaZipFileInfo fi = zipList.takeLast();
            list.push_back(std::unique_ptr<const ArchiveFileInfo>(
                               new ArchiveFileInfo(
                                   fi.name, fi.dateTime, fi.uncompressedSize)));
        }
        zip.close();
        if (zip.getZipError() != UNZ_OK)
        {
            qWarning("testRead(): zip.close(): %d", zip.getZipError());
            return 1;
        }
        return 0;
    }

    // Try to open as RAR
    if (ArchiveRar::loadlib())
    {
        if (ArchiveRar::getFileInfoList(path, list) == 0)
        {
            return 0;
        }
    }
    return 1;
}

int ArchiveExtractor::extract(const QString &archiveName,
                              const QString &fileName,
                              const QString &newFileName) const
{
    QString success = JlCompress::extractFile(archiveName,
                                              fileName,
                                              newFileName);

    if (success.isEmpty() && ArchiveRar::loadlib())
    {
        ArchiveRar::extract(archiveName, fileName, newFileName);
    }

    return 0;
}

int ArchiveExtractor::readFile(const QString &archiveName,
                               const QString &fileName,
                               QByteArray &buffer) const
{
    QFile zipFile(archiveName);
    QuaZip zip(&zipFile);
    if (zip.open(QuaZip::mdUnzip))
    {
        zip.setFileNameCodec("UTF-8");

        if (zip.setCurrentFile(fileName))
        {
            QuaZipFile file(&zip);
            if (file.open(QIODevice::ReadOnly))
            {
                buffer = file.readAll();
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }

    if (buffer.isEmpty())
    {
        if (ArchiveRar::loadlib())
        {
            // TODO: Check return value
            ArchiveRar::readFile(archiveName, fileName, buffer);
        }
    }

    return 0;
}
