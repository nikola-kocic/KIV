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
    PictureLoader();
    QImage getImage(std::unique_ptr<QBuffer> device) const override;
    QImage getThumbnail(std::unique_ptr<QBuffer> device, const QSize &thumbSize) const override;
    QImage styleThumbnail(const QImage &img, const QSize &thumb_size) const override;

private:
    QSize ThumbnailImageSize(const QSize &image_size,
                             const QSize &thumb_size) const;
};

#endif  // PICTURELOADER_H
