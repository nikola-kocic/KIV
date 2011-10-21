#include "pixmap_loader.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

#include <QtCore/qfile.h>
#include <QtCore/qbuffer.h>
#include <QtGui/qimagereader.h>
#include <QtCore/qcryptographichash.h>
#include <QtDebug>

QPixmap loadImage(const ZipInfo &fileInfo)
{
    if(fileInfo.zipFile == "")
    {
        return loadFromFile(fileInfo);
    }
    else
    {
        return loadFromZip(fileInfo);
    }
}

QPixmap loadFromFile(const ZipInfo &fileInfo)
{
    if(fileInfo.filePath == "")
    {
        return QPixmap(0,0);
    }
    if(fileInfo.thumbSize == 0)
    {
        return QPixmap(fileInfo.filePath);
    }
    else
    {
        QImageReader image_reader(fileInfo.filePath);
        image_reader.setScaledSize( ThumbnailImageSize( image_reader.size().width(), image_reader.size().height(), fileInfo.thumbSize ) );


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

QPixmap loadFromZip(const ZipInfo &fileInfo)
{
    if(fileInfo.filePath == "")
    {
        return QPixmap(0,0);
    }
//    qDebug() << fileInfo.filePath << fileInfo.zipFile;
    QFile zipFile(fileInfo.filePath);
    QuaZip zip(&zipFile);
    if(!zip.open(QuaZip::mdUnzip))
    {
        qWarning("testRead(): zip.open(): %d", zip.getZipError());
        return QPixmap(0,0);
    }
    zip.setFileNameCodec("UTF-8");

    zip.setCurrentFile(fileInfo.zipFile);

    QuaZipFile file(&zip);
    char c;
    if(!file.open(QIODevice::ReadOnly))
    {
        qWarning("testRead(): file.open(): %d", file.getZipError());
        return QPixmap(0,0);
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

    if(fileInfo.thumbSize == 0)
    {
        QPixmap pm;
        pm.loadFromData(out.buffer());

        return pm;
    }
    else
    {
        QImageReader image_reader(&out);
        image_reader.setScaledSize( ThumbnailImageSize( image_reader.size().width(), image_reader.size().height(), fileInfo.thumbSize ) );
        return QPixmap::fromImageReader(&image_reader);
    }
}

QSize ThumbnailImageSize ( int image_width, int image_height, int thumb_size )
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
