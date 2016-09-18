#include "picture_loader.h"

#include <QBuffer>
#include <QFile>
#include <QImageReader>
#include <QPainter>

//#define DEBUG_PICTURE_LOADER
#ifdef DEBUG_PICTURE_LOADER
#include "helper.h"
#endif

IPictureLoader::~IPictureLoader() {}

PictureLoader::PictureLoader()
{
}

QImage PictureLoader::getImage(std::unique_ptr<QBuffer> device) const
{
#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT;
#endif
    Q_ASSERT(device != nullptr);
    QImageReader image_reader(device.get());
    const QImage img = image_reader.read();

    return img;
}

QImage PictureLoader::getThumbnail(std::unique_ptr<QBuffer> device, const QSize &thumbSize) const
{
    Q_ASSERT(device != nullptr);
#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT << thumbSize;
#endif
    QImageReader image_reader(device.get());
    if (!thumbSize.isEmpty())
    {
        if (image_reader.size().height() > thumbSize.height()
            || image_reader.size().width() > thumbSize.width())
        {
            image_reader.setScaledSize(
                        ThumbnailImageSize(
                            image_reader.size(), thumbSize));
        }
    }
    const QImage img = image_reader.read();
    return styleThumbnail(img, thumbSize);
}

QImage PictureLoader::styleThumbnail(const QImage &img, const QSize &thumb_size) const
{
    QImage thumb(thumb_size.width() + 2,
                 thumb_size.height() + 2,
                 QImage::Format_ARGB32);
    thumb.fill(qRgba(255, 255, 255, 0));
    QPainter painter(&thumb);
    QPoint imgPoint((thumb.width() - img.width()) / 2,
                    (thumb.height() - img.height()) / 2);
    painter.setPen(Qt::lightGray);
    painter.drawImage(imgPoint, img);
    painter.drawRect(0, 0, thumb.width() - 1, thumb.height() - 1);
    painter.end();
    return thumb;
}

QSize PictureLoader::ThumbnailImageSize(const QSize &image_size,
                                        const QSize &thumb_size) const
{
    QSize result = image_size;
    result.scale(thumb_size, Qt::KeepAspectRatio);
#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT << "scaling image from" << image_size << "to" << result;
#endif
    return result;
}
