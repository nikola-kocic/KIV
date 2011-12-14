#ifndef PICTUREITEM_H
#define PICTUREITEM_H

#include "picture_loader.h"
#include "teximg.h"

#include <QGLWidget>
#include <QBoxLayout>
#include <QFutureWatcher>
#include <QTimer>
#include <QtConcurrentMap>
#include <QtConcurrentRun>

//#define DEBUG_PICTUREITEM

#ifdef DEBUG_PICTUREITEM
    #include <QDateTime>
#endif

class PictureItem : public QWidget
{
    Q_OBJECT
public:
    PictureItem(bool opengl, QWidget *parent = 0, Qt::WindowFlags f = 0);

    void setZoom(qreal z);
    qreal getZoom();
    QList<qreal> getDefaultZoomSizes() const;

    void setRotation(qreal r);
    qreal getRotation() const;

    void setLockMode(LockMode::Mode);
    LockMode::Mode getLockMode() const;

    void setHardwareAcceleration(bool b);
    bool getHardwareAcceleration() const;

    void setPixmap(const FileInfo &info);
    bool isPixmapNull() const;


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
        QPixmap m_pixmap_edited;
        PictureItem *m_picItem;
        QPixmap m_pixmap;

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
        PictureItem *m_picItem;

        qreal m_scaleX;
        qreal m_scaleY;
        int m_offsetX;
        int m_offsetY;
        QPoint m_lastPos;
        QVector < QVector <GLuint> > m_textures;
        QVector < QVector <GLuint> > m_old_textures;
        TexImg *m_texImg;

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

    PictureItemGL *m_imageDisplay_gl;
    PictureItemRaster *m_imageDisplay_raster;
    QFutureWatcher< QImage > *m_loader_image;
    QFutureWatcher< QImage > *m_loader_texture;
    int m_returnTexCount;
    bool m_opengl;
    QTimer *m_timer_scrollPage;
    bool m_flag_jumpToEnd;
    QList<qreal> m_defaultZoomSizes;
    qreal m_zoom_value;
    qreal m_rotation_value;
    bool m_pixmapNull;
    QPoint m_point_drag;
    LockMode::Mode m_lockMode;
    QColor m_color_clear;

    QPoint pointToOrigin(int width, int height);
    void avoidOutOfScreen();
    void drag(const QPoint &pt);
    void beginDrag(const QPoint &pt);
    void endDrag();
    void updateLockMode();
    void setPixmapNull(bool value);
    QRectF m_boundingRect;
    bool m_dragging;

#ifdef DEBUG_PICTUREITEM
    QTime t;
#endif



signals:
    void pageNext();
    void pagePrevious();
    void toggleFullscreen();
    void setFullscreen(bool);
    void zoomChanged(qreal current, qreal previous);
    void imageChanged();
    void updateCursor(Qt::CursorShape);
    void quit();
    void boss();

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
