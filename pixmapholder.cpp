#include "pixmapholder.h"

PixmapHolder::PixmapHolder()
{
    org = NULL;
    isnull = true;
}

void PixmapHolder::setOrgPixmap(const QPixmap &p)
{
    org = p;
    isnull = org.isNull();
}
QPixmap PixmapHolder::getOrgPixmap()
{
    return org;
}
bool PixmapHolder::isNull()
{
    return isnull;
}

//void PixmapHolder::setRotation(qreal r);
