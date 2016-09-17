#include "picture_loader.h"

#include <QBuffer>
#include <QFile>
#include <QImageReader>
#include <QPainter>

//#define DEBUG_PICTURE_LOADER
#ifdef DEBUG_PICTURE_LOADER
#include "helper.h"
#endif


PictureLoader::PictureLoader(std::unique_ptr<const IArchiveExtractor> archive_extractor)
    : m_archive_extractor(std::move(archive_extractor))
{
}

QIODevice* PictureLoader::getDevice(const FileInfo &info) const
{
    QIODevice* device = nullptr;
#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT << info.getPath();
#endif
    if (info.fileExists())
    {
        if (info.isInArchive())
        {
            device = getArchiveFileDevice(info);
        }
        else
        {
            device = getFileDevice(info);
        }
    }
    return device;
}

QImage PictureLoader::getImage(const FileInfo &info) const
{
    QIODevice* device = getDevice(info);
#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT << info.getPath();
#endif
    if (device == nullptr)
    {
        return QImage(0,0, QImage::Format_RGB32);
    }

    QImageReader image_reader(device);
    const QImage img = image_reader.read();
    delete device;

    return img;
}

QImage PictureLoader::getThumbnail(const ThumbnailInfo &thumb_info) const
{
#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT << thumb_info.getFileInfo().getPath() << thumb_info.getThumbSize();
#endif
    QIODevice* data = getDevice(thumb_info.getFileInfo());
    if (data == nullptr)
    {
        return QImage(0, 0, QImage::Format_RGB32);
    }
    else
    {
        QImageReader image_reader(data);
        if (!thumb_info.getThumbSize().isEmpty())
        {
            if (image_reader.size().height() > thumb_info.getThumbSize().height()
                || image_reader.size().width() > thumb_info.getThumbSize().width())
            {
                image_reader.setScaledSize(
                            PictureLoader::ThumbnailImageSize(
                                image_reader.size(), thumb_info.getThumbSize()));
            }
        }
        const QImage img = image_reader.read();
        delete data;
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

QIODevice* PictureLoader::getFileDevice(const FileInfo &file_info) const
{
    QFile* file = new QFile(file_info.getPath());
#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT << thumb_info.getFileInfo().getPath() << image_reader.format()
             << image_reader.supportedImageFormats();
#endif

    return file;
}


QIODevice* PictureLoader::getArchiveFileDevice(const FileInfo &file_info) const
{
    QByteArray buff;
    int success = m_archive_extractor->readFile(
                file_info.getContainerPath(),
                file_info.getArchiveImagePath(),
                buff);

    if (success != 0 || buff.isEmpty())
    {
        return nullptr;
    }

#ifdef DEBUG_PICTURE_LOADER
    DEBUGOUT << "finished reading from archive"
             << thumb_info.getFileInfo().getPath();
#endif
    QBuffer* out = new QBuffer();
    out->setData(buff);
    return out;
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
