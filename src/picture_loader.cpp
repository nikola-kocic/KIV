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

PictureLoader::PictureLoader(std::unique_ptr<const IArchiveExtractor> archive_extractor)
    : m_archive_extractor(std::move(archive_extractor))
{
}

std::unique_ptr<QBuffer> PictureLoader::getReadDevice(const FileInfo &info) const
{
    QByteArray data;
#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT << info.getPath();
#endif
    if (info.fileExists())
    {
        if (info.isInArchive())
        {
            data = getArchiveFileData(info);
        }
        else
        {
            data = getFileData(info);
        }
    }
    if (data.isEmpty())
    {
        return nullptr;
    }
    std::unique_ptr<QBuffer> out = std::unique_ptr<QBuffer>(new QBuffer());
    out->setData(data);
    return out;
}

QImage PictureLoader::getImage(const FileInfo &info) const
{
    std::unique_ptr<QBuffer> device = getReadDevice(info);
#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT << info.getPath();
#endif
    if (device == nullptr)
    {
        return QImage(0,0, QImage::Format_RGB32);
    }

    QImageReader image_reader(device.get());
    const QImage img = image_reader.read();

    return img;
}

QImage PictureLoader::getThumbnail(const ThumbnailInfo &thumb_info) const
{
#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT << thumb_info.getFileInfo().getPath() << thumb_info.getThumbSize();
#endif
    std::unique_ptr<QBuffer> data = getReadDevice(thumb_info.getFileInfo());
    if (data == nullptr)
    {
        return QImage(0, 0, QImage::Format_RGB32);
    }
    else
    {
        QImageReader image_reader(data.get());
        if (!thumb_info.getThumbSize().isEmpty())
        {
            if (image_reader.size().height() > thumb_info.getThumbSize().height()
                || image_reader.size().width() > thumb_info.getThumbSize().width())
            {
                image_reader.setScaledSize(
                            ThumbnailImageSize(
                                image_reader.size(), thumb_info.getThumbSize()));
            }
        }
        const QImage img = image_reader.read();
        return styleThumbnail(img, thumb_info.getThumbSize());
    }
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

QByteArray PictureLoader::getFileData(const FileInfo &file_info) const
{
    QFile file(file_info.getPath());
#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT << thumb_info.getFileInfo().getPath() << image_reader.format()
             << image_reader.supportedImageFormats();
#endif
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    file.close();
    return data;
}


QByteArray PictureLoader::getArchiveFileData(const FileInfo &file_info) const
{
    QByteArray buff;
    int success = m_archive_extractor->readFile(
                file_info.getContainerPath(),
                file_info.getArchiveImagePath(),
                buff);

    if (success != 0 || buff.isEmpty())
    {
        return QByteArray();
    }

#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT << "finished reading from archive"
             << thumb_info.getFileInfo().getPath();
#endif
    return buff;
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
