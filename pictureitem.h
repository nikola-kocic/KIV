#ifndef PICTUREITEM_H
#define PICTUREITEM_H

#include "pictureitem_raster.h"
#include "pictureitem_gl.h"
#include "pictureitem_shared.h"
#include "teximg.h"
#include "picture_loader.h"


#include <QtGui/qboxlayout.h>

class PictureItem : public QWidget
{
    Q_OBJECT
public:
    PictureItem(bool opengl, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    bool isPixmapNull();
    void setZoom(qreal z);
    qreal getZoom();
    void setRotation(qreal r);
    qreal getRotation();
    void setLockMode(LockMode::Mode);
    QVector<qreal> getDefaultZoomSizes();
    LockMode::Mode getLockMode();
    void setHardwareAcceleration(bool b);
    bool getHardwareAcceleration();
    void setPixmap(const FileInfo &info);

private:
    PictureItemGL *imageDisplayGL;
    PictureItemRaster *imageDisplayRaster;
    bool opengl;
    void initPictureItem();
    PictureItemShared *picItemShared;

signals:
    void pageNext();
    void pagePrevious();
    void toggleFullscreen();
    void zoomChanged();
    void imageChanged();

public slots:
    void zoomIn();
    void zoomOut();
    void fitToScreen();
    void fitWidth();
    void fitHeight();

private slots:
    void setMouseCursor(Qt::CursorShape);

protected:
    void wheelEvent(QWheelEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *);

};

#endif // PICTUREITEM_H
