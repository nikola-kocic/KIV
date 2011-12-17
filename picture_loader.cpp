#include "picture_loader.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

#include <QFile>
#include <QBuffer>
#include <QImageReader>
#include <QPainter>
//#include <QCryptographicHash>

//#define DEBUG_PICTURE_LOADER

#ifdef DEBUG_PICTURE_LOADER
#include <QDebug>
//#include <QTime>
#endif

QImage PictureLoader::getImage(const FileInfo &info)
{
    if (!info.fileExists())
    {
        return QImage(0,0);
    }
    else if (info.isZip())
    {
        return PictureLoader::getImageFromZip(ThumbnailInfo(info, QSize(0, 0)));
    }
    else
    {
        return PictureLoader::getImageFromFile(ThumbnailInfo(info, QSize(0, 0)));
    }
}

QImage PictureLoader::getThumbnail(const ThumbnailInfo &thumb_info)
{
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime() << "PictureLoader::getImage" << thumb_info.info.getFilePath() << thumb_info.thumbSize;
#endif
    if (!thumb_info.info.fileExists())
    {
        return QImage(0,0);
    }
    else if (thumb_info.info.isZip())
    {
        return PictureLoader::styleThumbnail(PictureLoader::getImageFromZip(thumb_info), thumb_info);
    }
    else
    {
        return PictureLoader::styleThumbnail(PictureLoader::getImageFromFile(thumb_info), thumb_info);
    }
}

QImage PictureLoader::styleThumbnail(const QImage &img, const ThumbnailInfo &thumb_info)
{
    QImage thumb(thumb_info.thumbSize.width() + 2, thumb_info.thumbSize.height() + 2, QImage::Format_ARGB32);
    thumb.fill(qRgba(0, 200, 0, 0));
    QPainter painter(&thumb);
    QPoint imgPoint((thumb.width() - img.width()) / 2, (thumb.height() - img.height()) / 2);
    painter.setPen(Qt::lightGray);
    painter.drawRect(0, 0, thumb.width() - 1, thumb.height() - 1);
    painter.drawImage(imgPoint, img);
    painter.end();
    return thumb;
}

QImage PictureLoader::getImageFromFile(const ThumbnailInfo &thumb_info)
{
    QImageReader image_reader(thumb_info.info.getFilePath());
//    qDebug() << image_reader.format();
    if (!thumb_info.thumbSize.isEmpty())
    {
        if (image_reader.size().height() > thumb_info.thumbSize.height() || image_reader.size().width() > thumb_info.thumbSize.width())
        {
            image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size(), thumb_info.thumbSize));
        }
    }
    return image_reader.read();
}

QImage PictureLoader::getImageFromZip(const ThumbnailInfo &thumb_info)
{
    QFile zipFile(thumb_info.info.containerPath);
    QuaZip zip(&zipFile);
    if (!zip.open(QuaZip::mdUnzip))
    {
        qWarning("testRead(): zip.open(): %d", zip.getZipError());
        return QImage(0, 0);
    }
    zip.setFileNameCodec("UTF-8");

    zip.setCurrentFile(thumb_info.info.zipImagePath());

    QuaZipFile file(&zip);
    char c;
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("testRead(): file.open(): %d", file.getZipError());
        return QImage(0, 0);
    }

    QBuffer out;
    out.open(QIODevice::WriteOnly);
    char buf[4096];
    int len = 0;
    while (file.getChar(&c))
    {
        buf[len++] = c;
        if (len >= 4096)
        {
            out.write(buf, len);
            len = 0;
        }
    }
    if (len > 0)
    {
        out.write(buf, len);
    }
    out.close();
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime() << "PictureLoader::getImageFromZip" << "finished reading from zip";
#endif
    QImageReader image_reader(&out);
    if (!thumb_info.thumbSize.isEmpty())
    {
        if (image_reader.size().height() > thumb_info.thumbSize.height() || image_reader.size().width() > thumb_info.thumbSize.width())
        {
            image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size(), thumb_info.thumbSize));
        }
    }
    return image_reader.read();
}

QSize PictureLoader::ThumbnailImageSize(const QSize &image_size, const QSize &thumb_size)
{

    float x_scale = (float)thumb_size.width() / image_size.width();
    float y_scale = (float)thumb_size.height() / image_size.height();

    int new_width;
    int new_height;

    if (x_scale < y_scale)
    {
        new_height = image_size.height() * x_scale;
        new_width = image_size.width() * x_scale;
    }
    else
    {
        new_height = image_size.height() * y_scale;
        new_width = image_size.width() * y_scale;
    }

    QSize result(new_width, new_height);
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime() << "PictureLoader::ThumbnailImageSize" << "scaling image from" << image_size << "to" << result << "x_scale" << x_scale << "y_scale" << y_scale;
#endif
    return result;
}

//        QFile file;
//        file.setFileName(filepath);
//        if (!file.open(QIODevice::ReadOnly))
//        {
//                // some Error handling is done here
//        }
//        qDebug() << QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md4).toHex();
