#ifndef PICTURELOADER_H
#define PICTURELOADER_H

#include "helper.h"

#include <QImage>

class ThumbnailInfo
{
public:
    ThumbnailInfo(FileInfo info_, QSize thumbSize_) : info(info_), thumbSize(thumbSize_) {}
    FileInfo info;
    QSize thumbSize;
};

class PictureLoader
{
public:
    static QImage getImage(const FileInfo &info);
    static QImage getThumbnail(const ThumbnailInfo &thumb_info);

private:
    static QSize ThumbnailImageSize(const QSize &image_size, const QSize &thumb_size);
    static QImage getImageFromZip(const ThumbnailInfo &thumb_info);
    static QImage getImageFromFile(const ThumbnailInfo &thumb_info);
    static QImage styleThumbnail(const QImage &img, const ThumbnailInfo &thumb_info);
};

#endif // PICTURELOADER_H
