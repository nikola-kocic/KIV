#ifndef PIXMAPHOLDER_H
#define PIXMAPHOLDER_H
#include <QtGui/QPixmap>
#include <QtCore/QSize>

class PixmapHolder
{
public:
    PixmapHolder();
    void setOrgPixmap(const QPixmap &p);
    QPixmap getOrgPixmap();
//    QSize getOrigSize();
    bool isNull();
//    void setRotation(qreal r);
private:
    QPixmap org;
    bool isnull;
};

#endif // PIXMAPHOLDER_H
