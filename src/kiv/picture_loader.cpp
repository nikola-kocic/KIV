#include "picture_loader.h"

#include <QBuffer>
#include <QFile>
#include <QImageReader>
#include <QPainter>

//#define DEBUG_PICTURE_LOADER
#ifdef DEBUG_PICTURE_LOADER
#include <QDebug>
//#include <QTime>
#endif

#include <quazip.h>
#include <quazipfile.h>

#include "models/unrar/archive_rar.h"


QImage PictureLoader::getImage(const FileInfo &info)
{
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureLoader::getImage" << info.getPath();
#endif
    if (!info.fileExists())
    {
        return QImage(0,0);
    }
    else if (info.isInArchive())
    {
        return PictureLoader::getImageFromArchive(ThumbnailInfo(info, QSize(0, 0)));
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
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureLoader::getThumbnail" << thumb_info.getFileInfo().getPath() << thumb_info.getThumbSize();
#endif
    if (!thumb_info.getFileInfo().fileExists())
    {
        return QImage(0,0);
    }
    else if (thumb_info.getFileInfo().isInArchive())
    {
        return PictureLoader::styleThumbnail(PictureLoader::getImageFromArchive(thumb_info), thumb_info.getThumbSize());
    }
    else
    {
        return PictureLoader::styleThumbnail(PictureLoader::getImageFromFile(thumb_info), thumb_info.getThumbSize());
    }
    return QImage(0,0);
}

QImage PictureLoader::styleThumbnail(const QImage &img, const QSize &thumb_size)
{
    QImage thumb(thumb_size.width() + 2, thumb_size.height() + 2, QImage::Format_ARGB32);
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
    QImageReader image_reader(thumb_info.getFileInfo().getPath());
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << thumb_info.getFileInfo().getPath() << image_reader.format() << image_reader.supportedImageFormats();
#endif
    if (!thumb_info.getThumbSize().isEmpty())
    {
        if (image_reader.size().height() > thumb_info.getThumbSize().height() || image_reader.size().width() > thumb_info.getThumbSize().width())
        {
            image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size(), thumb_info.getThumbSize()));
        }
    }
    return image_reader.read();
}


QImage PictureLoader::getImageFromArchive(const ThumbnailInfo &thumb_info)
{
    QByteArray buff;

    QFile zipFile(thumb_info.getFileInfo().getContainerPath());
    QuaZip zip(&zipFile);
    if (zip.open(QuaZip::mdUnzip))
    {
        zip.setFileNameCodec("UTF-8");

        if (zip.setCurrentFile(thumb_info.getFileInfo().zipImagePath()))
        {
            QuaZipFile file(&zip);
            if (file.open(QIODevice::ReadOnly))
            {
                buff = file.readAll();
            }
            else
            {
                return QImage(0, 0);
            }
        }
        else
        {
            return QImage(0, 0);
        }
    }

    if (buff.isEmpty())
    {
        if (ArchiveRar::loadlib())
        {
            // TODO: Check return value
            ArchiveRar::readFile(thumb_info.getFileInfo().getContainerPath(),
                                        thumb_info.getFileInfo().rarImagePath(),
                                        buff);
        }
    }

    if (buff.isEmpty())
    {
        return QImage(0, 0);
    }

#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureLoader::getImageFromArchive" << "finished reading from archive" << thumb_info.getFileInfo().getPath();
#endif
    QBuffer out(&buff);
    QImageReader image_reader(&out);

#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << image_reader.format() << image_reader.supportedImageFormats();
#endif
    if (!thumb_info.getThumbSize().isEmpty())
    {
        if (image_reader.size().height() > thumb_info.getThumbSize().height() || image_reader.size().width() > thumb_info.getThumbSize().width())
        {
            image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size(), thumb_info.getThumbSize()));
        }
    }
    const QImage img = image_reader.read();

    return img;
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
