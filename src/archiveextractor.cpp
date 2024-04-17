#include "archiveextractor.h"

#include <QFile>

#include "models/unrar/archive_rar.h"
#include <quazip/JlCompress.h>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

IArchiveExtractor::~IArchiveExtractor() {}

ArchiveExtractor::ArchiveExtractor() {}

std::optional<std::vector<ArchiveFileInfo>> ArchiveExtractor::getFileInfoList(
    const QString &path) const
{
    QFile archiveFile(path);

    // Try to open as ZIP
    QuaZip zip(&archiveFile);
    if (zip.open(QuaZip::mdUnzip)) {
        zip.setFileNameCodec("UTF-8");
        auto zipList = zip.getFileInfoList();
        auto list = std::vector<ArchiveFileInfo>{};
        list.reserve(zipList.size());
        std::reverse(zipList.begin(), zipList.end());
        while (!zipList.empty()) {
            const QuaZipFileInfo fi = zipList.takeLast();
            list.emplace_back(fi.name, fi.dateTime, fi.uncompressedSize);
        }
        zip.close();
        if (zip.getZipError() != UNZ_OK) {
            qWarning("testRead(): zip.close(): %d", zip.getZipError());
        }
        return list;
    }

    // Try to open as RAR
    if (ArchiveRar::loadlib()) {
        return ArchiveRar::getFileInfoList(path);
    }
    return std::nullopt;
}

int ArchiveExtractor::extract(
    const QString &archiveName, const QString &fileName, const QString &newFileName) const
{
    QString success = JlCompress::extractFile(archiveName, fileName, newFileName);

    if (success.isEmpty() && ArchiveRar::loadlib()) {
        ArchiveRar::extract(archiveName, fileName, newFileName);
    }

    return 0;
}

std::optional<QByteArray> ArchiveExtractor::readFile(
    const QString &archiveName, const QString &fileName, int maxSize) const
{
    QFile zipFile(archiveName);
    QuaZip zip(&zipFile);
    if (zip.open(QuaZip::mdUnzip)) {
        zip.setFileNameCodec("UTF-8");
        if (!zip.setCurrentFile(fileName)) {
            return std::nullopt;
        }
        QuaZipFile file(&zip);
        if (!file.open(QIODevice::ReadOnly)) {
            return std::nullopt;
        }
        QByteArray buffer;
        if (maxSize == -1) {
            buffer = file.readAll();
        } else {
            buffer = file.read(maxSize);
        }
        return buffer;
    }

    if (ArchiveRar::loadlib()) {
        // TODO: Check return value
        return ArchiveRar::readFile(archiveName, fileName, maxSize);
    }

    return std::nullopt;
}
