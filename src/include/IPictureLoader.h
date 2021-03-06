#ifndef IPICTURELOADER_H
#define IPICTURELOADER_H

#include "fileinfo.h"

#include <QImage>
#include <QBuffer>

#include <memory>

class ThumbnailInfo
{
public:
    explicit ThumbnailInfo(const FileInfo &info, const QSize &thumbSize)
        : m_info(info)
        , m_thumbSize(thumbSize)
    {}

    inline FileInfo getFileInfo() const { return m_info; }
    inline QSize getThumbSize() const { return m_thumbSize; }

private:
    const FileInfo m_info;
    const QSize m_thumbSize;
};

class IPictureLoader
{
public:
    // Empty virtual destructor for proper cleanup
    virtual ~IPictureLoader();
    virtual QImage getImage(std::unique_ptr<QBuffer> device) const = 0;
    virtual QImage getThumbnail(std::unique_ptr<QBuffer> device, const QSize &thumbSize) const = 0;
    virtual QImage styleThumbnail(const QImage &img, const QSize &thumb_size) const = 0;
};

#endif // IPICTURELOADER_H
