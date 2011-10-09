#ifndef PICTUREITEMRASTER_H
#define PICTUREITEMRASTER_H

#include "komicviewer_enums.h"
#include "pictureitem_shared.h"

#include <QtCore/qtimer.h>
#include <QtGui/qwidget.h>

class PictureItemRaster : public QWidget
{
    Q_OBJECT

public:
    PictureItemRaster(PictureItemShared* pis, QWidget * parent = 0, Qt::WindowFlags f = 0 );
    void setRotation( qreal r );

private:
    QPixmap pixmap_edited;
    PictureItemShared* pis;
//    MiddleClick::Action middleClickAction;
//    Wheel::Action wheelAction;
//    bool LockDrag;

public slots:
    void setZoom();

private slots:
    void setPixmap();

protected:
    void paintEvent( QPaintEvent *event );

signals:
    void zoomChanged();

};

#endif // PICTUREITEMRASTER_H
