#include "pixmap_loader.h"
#include <QtCore/qfile.h>
#include <QtCore/qbuffer.h>

PixmapLoader::PixmapLoader()
{
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
    emit finished(QPixmap(filepath));
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

    QPixmap pm;
    pm.loadFromData(out.buffer());

    emit finished(pm);
}
