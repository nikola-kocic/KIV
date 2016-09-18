#ifndef PICTURELOADER_H
#define PICTURELOADER_H

#include <QAbstractItemModel>
#include <QImage>
#include <QBuffer>

#include "include/IPictureLoader.h"
#include "helper.h"

class PictureLoader: public IPictureLoader
{
public:
    PictureLoader(std::unique_ptr<const IArchiveExtractor> archive_extractor);
    QImage getImage(const FileInfo &info) const override;
    QImage getThumbnail(const ThumbnailInfo &thumb_info) const override;
    QImage styleThumbnail(const QImage &img, const QSize &thumb_size) const override;

private:
    QSize ThumbnailImageSize(const QSize &image_size,
                             const QSize &thumb_size) const;
    std::unique_ptr<QBuffer> getReadDevice(const FileInfo &info) const;
    QByteArray getArchiveFileData(const FileInfo &file_info) const;
    QByteArray getFileData(const FileInfo &file_info) const;

    std::unique_ptr<const IArchiveExtractor> m_archive_extractor;
};

#endif  // PICTURELOADER_H
