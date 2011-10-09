#ifndef PICTUREITEMGL_H
#define PICTUREITEMGL_H

#include "komicviewer_enums.h"
#include "pictureitem_shared.h"

#include <QtCore/qtimer.h>
#include <QtOpenGL/qgl.h>

class PictureItemGL : public QGLWidget
{
    Q_OBJECT

public:
    PictureItemGL( PictureItemShared* pis, QWidget * parent = 0, Qt::WindowFlags f = 0 );
    void setRotation(qreal r);

private:
    PictureItemShared* pis;
    QPointF translatePoint;

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void zoomChanged();

public slots:
    void setZoom(qreal current, qreal previous);

private slots:
    void setPixmap();

};


#endif // PICTUREITEMGL_H
