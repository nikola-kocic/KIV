#ifndef PIXMAPLOADER_H
#define PIXMAPLOADER_H

#include "komicviewer_enums.h"

#include <QtCore/qobject.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qicon.h>

class PixmapLoader
{
public:
    static QPixmap getPixmap(const FileInfo &info);
    static QImage getImage(const FileInfo &info);

private:
    static QPixmap getPixmapFromFile(const FileInfo &info);
    static QPixmap getPixmapFromZip(const FileInfo &info);
    static QSize ThumbnailImageSize(int image_width, int image_height, int thumb_size);
    static QImage getImageFromZip(const FileInfo &info);
    static QImage getImageFromFile(const FileInfo &info);
};

#endif // PIXMAPLOADER_H
