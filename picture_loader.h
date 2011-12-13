#ifndef PICTURELOADER_H
#define PICTURELOADER_H

#include "helper.h"

#include <QImage>

class PictureLoader
{
public:
    static QImage getImage(const FileInfo &info);

private:
    static QSize ThumbnailImageSize(int image_width, int image_height, int thumb_size);
    static QImage getImageFromZip(const FileInfo &info);
    static QImage getImageFromFile(const FileInfo &info);
};

#endif // PICTURELOADER_H
