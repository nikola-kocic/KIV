#include "picture_loader.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

//#include <QDebug>
//#include <QTime>
#include <QFile>
#include <QBuffer>
#include <QImageReader>
//#include <QCryptographicHash>

QImage PictureLoader::getImage(const FileInfo &info)
{
//    qDebug() << info.containerPath << info.imageFileName << info.thumbSize << info.zipImageFileName << info.zipPathToImage;
    if (!info.fileExists())
    {
        return QImage(0,0);
    }
    else if (info.isZip())
    {
        return PictureLoader::getImageFromZip(info);
    }
    else
    {
        return PictureLoader::getImageFromFile(info);
    }
}

QImage PictureLoader::getImageFromFile(const FileInfo &info)
{
    QImageReader image_reader(info.getFilePath());
//    qDebug() << image_reader.format();
    if (info.thumbSize != 0)
    {
        image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size().width(), image_reader.size().height(), info.thumbSize));
    }
    return image_reader.read();
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

    zip.setCurrentFile(info.zipImagePath());

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

    QImageReader image_reader(&out);
    if (info.thumbSize != 0)
    {
        image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size().width(), image_reader.size().height(), info.thumbSize));
    }
    return image_reader.read();
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

//        QFile file;
//        file.setFileName(filepath);
//        if (!file.open(QIODevice::ReadOnly))
//        {
//                // some Error handling is done here
//        }
//        qDebug() << QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md4).toHex();
