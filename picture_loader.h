#ifndef PICTURELOADER_H
#define PICTURELOADER_H

#include "helper.h"

#include <QImage>
#include <QAbstractItemModel>

class ThumbnailInfo
{
public:
    explicit ThumbnailInfo(const FileInfo &info, const QSize &thumbSize)
        : m_info(info)
        , m_thumbSize(thumbSize)
    {}

    inline FileInfo getFileInfo() const { return m_info; }
    inline QSize getThumbSize() const { return m_thumbSize; }

private:
    const FileInfo m_info;
    const QSize m_thumbSize;
};

class PictureLoader
{
public:
    static QImage getImage(const FileInfo &info);
    static QImage getThumbnail(const ThumbnailInfo &thumb_info);
    static QImage styleThumbnail(const QImage &img, const QSize &thumb_size);

private:
    static QSize ThumbnailImageSize(const QSize &image_size, const QSize &thumb_size);
    static QImage getImageFromArchive(const ThumbnailInfo &thumb_info);
    static QImage getImageFromFile(const ThumbnailInfo &thumb_info);
};

#endif // PICTURELOADER_H
