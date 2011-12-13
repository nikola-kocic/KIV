#ifndef PICTUREITEM_H
#define PICTUREITEM_H

#include "picture_loader.h"
#include "teximg.h"

#include <QtOpenGL/qgl.h>
#include <QtGui/qboxlayout.h>
#include <QtCore/qfuturewatcher.h>
#include <QtCore/qtimer.h>
#include <QtCore/qtconcurrentmap.h>
#include <QtCore/qtconcurrentrun.h>

//#define DEBUG_PICTUREITEM

#ifdef DEBUG_PICTUREITEM
    #include <qdatetime.h>
#endif

class PictureItem : public QWidget
{
    Q_OBJECT
public:
    PictureItem(bool opengl, QWidget *parent = 0, Qt::WindowFlags f = 0);

    void setZoom(qreal z);
    qreal getZoom();
    QList<qreal> getDefaultZoomSizes();

    void setRotation(qreal r);
    qreal getRotation();

    void setLockMode(LockMode::Mode);
    LockMode::Mode getLockMode();

    void setHardwareAcceleration(bool b);
    bool getHardwareAcceleration();

    void setPixmap(const FileInfo &info);
    bool isPixmapNull();


private:
    class PictureItemRaster : public QWidget
    {
    public:
        PictureItemRaster(PictureItem *parent, Qt::WindowFlags f = 0);
        void setRotation(qreal r);
        void setFile(const FileInfo &info);
        void setZoom(qreal current, qreal previous);
        void setImage(QImage img);

    private:
        QPixmap pixmap_edited;
        PictureItem *picItem;
        QPixmap pixmap;

    protected:
        void paintEvent(QPaintEvent *event);

    };

    class PictureItemGL : public QGLWidget
    {
    public:
        PictureItemGL(PictureItem *parent, Qt::WindowFlags f = 0);
        ~PictureItemGL();
        void setRotation(qreal r);
        void setClearColor(const QColor &color);
        void setZoom(qreal current, qreal previous);
        void setImage(QImage img);
        void setTexture(QImage tex, int num);
        void textureLoadFinished();

    private:
        void updateSize();
        void clearTextures();
        PictureItem *picItem;

        qreal scaleX;
        qreal scaleY;
        int offsetX;
        int offsetY;
        QColor clearColor;
        QPoint lastPos;
        QVector < QVector <GLuint> > textures;
        QVector < QVector <GLuint> > old_textures;
        TexImg *texImg;

    protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

    };

    void initPictureItem();
    void start_timerScrollPage();
    void afterPixmapLoad();
    void ScrollPageHorizontal(int value);
    void ScrollPageVertical(int value);

    PictureItemGL *imageDisplayGL;
    PictureItemRaster *imageDisplayRaster;
    QFutureWatcher< QImage > *imageLoader;
    QFutureWatcher< QImage > *textureLoader;
    int returnTexCount;
    bool opengl;
    QTimer *timerScrollPage;
    bool flagJumpToEnd;
    QList<qreal> defaultZoomSizes;
    qreal zoom;
    qreal rotation;
    bool pixmapNull;
    QPoint dragPoint;
    LockMode::Mode lockMode;

    QPoint pointToOrigin(int width, int height);
    void avoidOutOfScreen();
    void drag(const QPoint &pt);
    void beginDrag(const QPoint &pt);
    void endDrag();
    void updateLockMode();
    bool dragging;
    void setPixmapNull(bool value);
    QRectF boundingRect;

#ifdef DEBUG_PICTUREITEM
    QTime t;
#endif



signals:
    void pageNext();
    void pagePrevious();
    void toggleFullscreen();
    void zoomChanged(qreal current, qreal previous);
    void imageChanged();
    void updateCursor(Qt::CursorShape);

public slots:
    void zoomIn();
    void zoomOut();
    void fitToScreen();
    void fitWidth();
    void fitHeight();

private slots:
    void textureFinished(int num);
    void imageFinished(int num);

protected:
    void loadTextures(QList<TexIndex> indexes);
    void wheelEvent(QWheelEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *);
};

#endif // PICTUREITEM_H
