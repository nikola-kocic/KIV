#include "picture_loader.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

//#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qbuffer.h>
#include <QtGui/qimagereader.h>
#include <QtCore/qcryptographichash.h>

QPixmap PictureLoader::getPixmap(const FileInfo &info)
{
//    qDebug() << info.containerPath << info.imageFileName << info.thumbSize << info.zipImageFileName << info.zipPathToImage;
    if (info.zipImageFileName.isEmpty())
    {
        if (info.imageFileName.isEmpty())
        {
            return QPixmap(0,0);
        }
        else
        {
            return PictureLoader::getPixmapFromFile(info);
        }
    }
    else
    {
        if (info.containerPath.isEmpty())
        {
            return QPixmap(0, 0);
        }
        else
        {
            return PictureLoader::getPixmapFromZip(info);
        }
    }
}

QImage PictureLoader::getImage(const FileInfo &info)
{
//    qDebug() << info.containerPath << info.imageFileName << info.thumbSize << info.zipImageFileName << info.zipPathToImage;
    if (info.zipImageFileName.isEmpty())
    {
        if (info.imageFileName.isEmpty())
        {
            return QImage(0,0);
        }
        else
        {
            return PictureLoader::getImageFromFile(info);
        }
    }
    else
    {
        if (info.containerPath.isEmpty())
        {
            return QImage(0, 0);
        }
        else
        {
            return PictureLoader::getImageFromZip(info);
        }
    }
}

QPixmap PictureLoader::getPixmapFromFile(const FileInfo &info)
{
    if (info.thumbSize == 0)
    {
        return QPixmap(info.containerPath + "/" + info.imageFileName);
    }
    else
    {
        QImageReader image_reader(info.containerPath + "/" + info.imageFileName);
        image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size().width(), image_reader.size().height(), info.thumbSize));

//        QFile file;
//        file.setFileName(filepath);
//        if (!file.open(QIODevice::ReadOnly))
//        {
//                // some Error handling is done here
//        }
//        qDebug() << QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md4).toHex();

        return QPixmap::fromImageReader(&image_reader);
    }
}

QImage PictureLoader::getImageFromFile(const FileInfo &info)
{
    if (info.thumbSize == 0)
    {
        return QImage(info.containerPath + "/" + info.imageFileName);
    }
    else
    {
        QImageReader image_reader(info.containerPath + "/" + info.imageFileName);
        image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size().width(), image_reader.size().height(), info.thumbSize));
        return image_reader.read();
    }
}

QPixmap PictureLoader::getPixmapFromZip(const FileInfo &info)
{
    QFile zipFile(info.containerPath);
    QuaZip zip(&zipFile);
    if (!zip.open(QuaZip::mdUnzip))
    {
        qWarning("testRead(): zip.open(): %d", zip.getZipError());
        return QPixmap(0, 0);
    }
    zip.setFileNameCodec("UTF-8");

    zip.setCurrentFile((info.zipPathToImage.compare("/") == 0 ? "" : info.zipPathToImage) + info.zipImageFileName);

    QuaZipFile file(&zip);
    char c;
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("testRead(): file.open(): %d", file.getZipError());
        return QPixmap(0, 0);
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

    if (info.thumbSize == 0)
    {
        QPixmap pm;
        pm.loadFromData(out.buffer());
        return pm;
    }
    else
    {
        QImageReader image_reader(&out);
        image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size().width(), image_reader.size().height(), info.thumbSize));
        return QPixmap::fromImageReader(&image_reader);
    }
}

QImage PictureLoader::getImageFromZip(const FileInfo &info)
{
    QFile zipFile(info.containerPath);
    QuaZip zip(&zipFile);
    if (!zip.open(QuaZip::mdUnzip))
    {
        qWarning("testRead(): zip.open(): %d", zip.getZipError());
        return QImage(0, 0);
    }
    zip.setFileNameCodec("UTF-8");

    zip.setCurrentFile((info.zipPathToImage.compare("/") == 0 ? "" : info.zipPathToImage) + info.zipImageFileName);

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

    if (info.thumbSize == 0)
    {
        QImage img;
        img.loadFromData(out.buffer());
        return img;
    }
    else
    {
        QImageReader image_reader(&out);
        image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size().width(), image_reader.size().height(), info.thumbSize));
        return image_reader.read();
    }
}

QSize PictureLoader::ThumbnailImageSize(int image_width, int image_height, int thumb_size)
{
    if (image_width > image_height)
    {
        image_height = static_cast<double>(thumb_size) / image_width * image_height;
        image_width = thumb_size;
    }
    else if (image_width < image_height)
    {
        image_width = static_cast<double>(thumb_size) / image_height * image_width;
        image_height = thumb_size;
    }
    else
    {
        image_width = thumb_size;
        image_height = thumb_size;
    }
    return QSize(image_width, image_height);
}
