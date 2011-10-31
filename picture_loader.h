#ifndef PICTURELOADER_H
#define PICTURELOADER_H

#include "komicviewer_enums.h"

#include <QtGui/qpixmap.h>

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
