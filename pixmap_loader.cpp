#include "pixmap_loader.h"
#include <QtCore/qfile.h>
#include <QtCore/qbuffer.h>
#include <QtGui/qimagereader.h>

PixmapLoader::PixmapLoader()
{
    this->length = 0;
    this->filepath = "/";
}

void PixmapLoader::setFilePath(const QString &filepath, bool isZip, const QString &zipFileName)
{
    this->filepath = filepath;
    this->isZip = isZip;
    this->zipFileName = zipFileName;
}

QString PixmapLoader::getFilePath()
{
    return this->filepath;
}

void PixmapLoader::setThumbnailSize(int length)
{
    this->length = length;
}

void PixmapLoader::loadPixmap()
{
    if(this->isZip == true)
    {
        loadFromZip();
    }
    else
    {
        loadFromFile();
    }
}

void PixmapLoader::loadFromFile()
{
    if(this->length == 0)
    {
        emit finished(QPixmap(filepath));
    }
    else
    {
        QImageReader image_reader(filepath);
        image_reader.setScaledSize( ThumbnailImageSize( image_reader.size().width(), image_reader.size().height() ) );
        QIcon icon;
        icon.addPixmap(QPixmap::fromImageReader(&image_reader));
        emit finished(icon);
    }
    return;
}

void PixmapLoader::loadFromZip()
{
    QFile zipFile(this->filepath);
    QuaZip zip(&zipFile);
    if(!zip.open(QuaZip::mdUnzip))
    {
        qWarning("testRead(): zip.open(): %d", zip.getZipError());
        return;
    }
    zip.setFileNameCodec("UTF-8");
    zip.setCurrentFile(this->zipFileName);

    QuaZipFile file(&zip);
    char c;
    if(!file.open(QIODevice::ReadOnly))
    {
        qWarning("testRead(): file.open(): %d", file.getZipError());
        return;
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

    if(this->length == 0)
    {
        QPixmap pm;
        pm.loadFromData(out.buffer());

        emit finished(pm);
    }
    else
    {
        QImageReader image_reader(&out);
        image_reader.setScaledSize( ThumbnailImageSize( image_reader.size().width(), image_reader.size().height() ) );
        QIcon icon;
        QPixmap p = QPixmap::fromImageReader(&image_reader);
        icon.addPixmap(p);
        emit finished(icon);
    }
}

QSize PixmapLoader::ThumbnailImageSize ( int image_width, int image_height )
{
    if (image_width > image_height)
    {
        image_height = static_cast<double>(this->length) / image_width * image_height;
        image_width = this->length;
    }
    else if (image_width < image_height)
    {
        image_width = static_cast<double>(this->length) / image_height * image_width;
        image_height = this->length;
    }
    else
    {
        image_width = this->length;
        image_height = this->length;
    }
    return QSize(image_width, image_height);
}
