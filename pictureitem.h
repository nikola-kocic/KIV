#ifndef PICTUREITEM_H
#define PICTUREITEM_H

#include "pictureitem_raster.h"
#include "pictureitem_gl.h"
#include "pictureitem_shared.h"

#include <QtGui/qboxlayout.h>

class PictureItem : public QWidget
{
    Q_OBJECT
public:
    PictureItem(bool opengl, QWidget * parent = 0, Qt::WindowFlags f = 0 );

    QPixmap getPixmap();
    void setZoom(qreal z);
    qreal getZoom();
    void setRotation(qreal r);
    qreal getRotation();
    void setLockMode(LockMode::Mode);
    QVector<qreal> getDefaultZoomSizes();
    LockMode::Mode getLockMode();
    void setHardwareAcceleration(bool b);
    bool getHardwareAcceleration();

private:
    PictureItemGL *imageDisplayGL;
    PictureItemRaster *imageDisplayRaster;
    bool opengl;
    void initPictureItem();
    QVBoxLayout *vboxMain;
    PictureItemShared* pis;

signals:
    void pageNext();
    void pagePrevious();
    void toggleFullscreen();
    void zoomChanged();

public slots:
    void zoomIn();
    void zoomOut();
    void fitToScreen();
    void fitWidth();
    void fitHeight();
    void setPixmap(const QPixmap &p);

private slots:
    void setMouseCursor(Qt::CursorShape);

protected:
    void wheelEvent( QWheelEvent* );
    void mousePressEvent( QMouseEvent *ev );
    void mouseMoveEvent( QMouseEvent *ev );
    void mouseReleaseEvent( QMouseEvent *ev );
    void resizeEvent( QResizeEvent * );
    void keyPressEvent( QKeyEvent *ev );

};

#endif // PICTUREITEM_H
