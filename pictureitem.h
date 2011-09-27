#ifndef PICTUREITEMWIDGET_H
#define PICTUREITEMWIDGET_H

#include "pictureitem_raster.h"
#include "pictureitem_gl.h"

class PictureItem : public QWidget
{
    Q_OBJECT
public:
    PictureItem(bool opengl, QWidget * parent = 0, Qt::WindowFlags f = 0 );

    QPixmap getPixmap();
    void setZoom(qreal z);
    qreal getZoom();
    void setPixmap(const QPixmap &p);
    void setRotation(qreal r);
    qreal getRotation();
    void setLockMode(LockMode::Mode);
    QVector<qreal> getDefaultZoomSizes();
    LockMode::Mode getLockMode();

private:
    PictureItemGL *imageDisplayGL;
    PictureItemRaster *imageDisplayRaster;
    bool opengl;

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

};

#endif // PICTUREITEMWIDGET_H
