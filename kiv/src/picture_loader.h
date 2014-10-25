#ifndef PICTURELOADER_H
#define PICTURELOADER_H

#include <QAbstractItemModel>
#include <QImage>

#include "kiv/include/IPictureLoader.h"
#include "kiv/src/helper.h"

class PictureLoader : public IPictureLoader
{
public:
    PictureLoader(IArchiveExtractor *archive_extractor);
    QImage getImage(const FileInfo &info) const;
    QImage getThumbnail(const ThumbnailInfo &thumb_info) const;
    QImage styleThumbnail(const QImage &img, const QSize &thumb_size) const;

private:
    QSize ThumbnailImageSize(const QSize &image_size,
                             const QSize &thumb_size) const;
    QImage getImageFromArchive(const ThumbnailInfo &thumb_info) const;
    QImage getImageFromFile(const ThumbnailInfo &thumb_info) const;

    IArchiveExtractor *m_archive_extractor;
};

#endif  // PICTURELOADER_H
