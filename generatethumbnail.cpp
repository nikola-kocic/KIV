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
    QImageReader image_reader(path);
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
    image_reader.setScaledSize(QSize(image_width, image_height));
    QIcon icon;
    icon.addPixmap(QPixmap::fromImage(image_reader.read()));

    IconInfo ii;
    ii.index = index;
    ii.icon = icon;
    emit finished(ii);
}
