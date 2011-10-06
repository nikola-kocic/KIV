#include "generatethumbnail.h"

#include <QtGui>
#include <QtCore>

generateThumbnail::generateThumbnail(const QString& path, int length, int index)
{
    this->path = path;
    this->length = length;
    this->index = index;
}

void generateThumbnail::returnThumbnail()
{
    bool error = false;
    QImageReader image_reader(path);
//    qDebug() << "image_reader";
    if(!image_reader.canRead()) error = true;
    int image_width = image_reader.size().width();
    int image_height = image_reader.size().height();
    if (image_width > image_height) {
      image_height = static_cast<double>(length) / image_width * image_height;
      image_width = length;
    } else if (image_width < image_height) {
      image_width = static_cast<double>(length) / image_height * image_width;
      image_height = length;
    } else {
      image_width = length;
      image_height = length;
    }
//    qDebug() << "image_height";
    image_reader.setScaledSize(QSize(image_width, image_height));
//    qDebug() << "setScaledSize";
    QIcon icon;
    icon.addPixmap(QPixmap::fromImage(image_reader.read()));
//    qDebug() << "addPixmap";
    IconInfo ii;
    ii.index = index;
    ii.icon = icon;
    if(ii.icon.isNull())
    {
        error = true;
    }
    ii.error = error;
//    qDebug() << "finished";
    emit finished(ii);
    return;
}
