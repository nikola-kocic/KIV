#include "picture_loader.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

#include <QFile>
#include <QBuffer>
#include <QImageReader>
#include <QPainter>

//#define DEBUG_PICTURE_LOADER

#include <QDebug>
#ifdef DEBUG_PICTURE_LOADER
#include <QDebug>
//#include <QTime>
#endif

QImage PictureLoader::getImage(const FileInfo &info)
{
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureLoader::getImage" << info.getFilePath();
#endif
    if (!info.fileExists())
    {
        return QImage(0,0);
    }
    else if (info.isArchive())
    {
        return PictureLoader::getImageFromZip(ThumbnailInfo(info, QSize(0, 0)));
    }
    else
    {
        return PictureLoader::getImageFromFile(ThumbnailInfo(info, QSize(0, 0)));
    }
    return QImage(0,0);
}

QImage PictureLoader::getThumbnail(const ThumbnailInfo &thumb_info)
{
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureLoader::getThumbnail" << thumb_info.getFileInfo().getFilePath() << thumb_info.getThumbSize();
#endif
    if (!thumb_info.getFileInfo().fileExists())
    {
        return QImage(0,0);
    }
    else if (thumb_info.getFileInfo().isArchive())
    {
        return PictureLoader::styleThumbnail(PictureLoader::getImageFromZip(thumb_info), thumb_info);
    }
    else
    {
        return PictureLoader::styleThumbnail(PictureLoader::getImageFromFile(thumb_info), thumb_info);
    }
    return QImage(0,0);
}

QImage PictureLoader::styleThumbnail(const QImage &img, const ThumbnailInfo &thumb_info)
{
    QImage thumb(thumb_info.getThumbSize().width() + 2, thumb_info.getThumbSize().height() + 2, QImage::Format_ARGB32);
    thumb.fill(qRgba(255, 255, 255, 0));
    QPainter painter(&thumb);
    QPoint imgPoint((thumb.width() - img.width()) / 2, (thumb.height() - img.height()) / 2);
    painter.setPen(Qt::lightGray);
    painter.drawImage(imgPoint, img);
    painter.drawRect(0, 0, thumb.width() - 1, thumb.height() - 1);
    painter.end();
    return thumb;
}

QImage PictureLoader::getImageFromFile(const ThumbnailInfo &thumb_info)
{
    QImageReader image_reader(thumb_info.getFileInfo().getFilePath());
//    qDebug() << image_reader.format();
    if (!thumb_info.getThumbSize().isEmpty())
    {
        if (image_reader.size().height() > thumb_info.getThumbSize().height() || image_reader.size().width() > thumb_info.getThumbSize().width())
        {
            image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size(), thumb_info.getThumbSize()));
        }
    }
    return image_reader.read();
}

QImage PictureLoader::getImageFromZip(const ThumbnailInfo &thumb_info)
{
    QFile zipFile(thumb_info.getFileInfo().container.canonicalFilePath());
    QuaZip zip(&zipFile);
    if (!zip.open(QuaZip::mdUnzip))
    {
        qWarning("testRead(): zip.open(): %d", zip.getZipError());
        return QImage(0, 0);
    }
    zip.setFileNameCodec("UTF-8");

    zip.setCurrentFile(thumb_info.getFileInfo().zipImagePath());

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
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureLoader::getImageFromZip" << "finished reading from zip";
#endif
    QImageReader image_reader(&out);
    if (!thumb_info.getThumbSize().isEmpty())
    {
        if (image_reader.size().height() > thumb_info.getThumbSize().height() || image_reader.size().width() > thumb_info.getThumbSize().width())
        {
            image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size(), thumb_info.getThumbSize()));
        }
    }
    return image_reader.read();
}

QSize PictureLoader::ThumbnailImageSize(const QSize &image_size, const QSize &thumb_size)
{
    QSize result = image_size;
    result.scale(thumb_size, Qt::KeepAspectRatio);
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureLoader::ThumbnailImageSize" << "scaling image from" << image_size << "to" << result;
#endif
    return result;
}
