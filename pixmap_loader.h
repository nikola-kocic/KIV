#ifndef PIXMAPLOADER_H
#define PIXMAPLOADER_H

#include <QtCore/qobject.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qicon.h>
#include "komicviewer_enums.h"

QPixmap loadImage(const ZipInfo &fileInfo);
QPixmap loadFromFile(const ZipInfo &fileInfo);
QPixmap loadFromZip(const ZipInfo &fileInfo);
QSize ThumbnailImageSize ( int image_width, int image_height, int thumb_size );

#endif // PIXMAPLOADER_H
