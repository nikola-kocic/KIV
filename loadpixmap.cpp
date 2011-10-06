#include "loadpixmap.h"

loadPixmap::loadPixmap(const QString& path)
{
    this->path = path;
}

void loadPixmap::loadFromFile()
{
    emit finished(QPixmap(path));
    return;
}
