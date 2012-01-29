#ifndef PICTUREITEM_H
#define PICTUREITEM_H

#include "picture_loader.h"
#include "teximg.h"
#include "settings.h"

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
    explicit PictureItem(Settings *settings, QWidget *parent = 0, Qt::WindowFlags f = 0);

    void setZoom(const qreal z);
    qreal getZoom() const;
    QList<qreal> getDefaultZoomSizes() const;

    void setRotation(const qreal r);
    qreal getRotation() const;

    void setLockMode(const LockMode::Mode &mode);
    LockMode::Mode getLockMode() const;

    void setHardwareAcceleration(const bool b);
    bool getHardwareAcceleration() const;

    void setPixmap(const FileInfo &info);
    bool isPixmapNull() const;


private:
    class PictureItemRaster : public QWidget
    {
    public:
        explicit PictureItemRaster(PictureItem* parent, Qt::WindowFlags f = 0);
        void setRotation(const qreal current, const qreal previous);
        void setFile(const FileInfo &info);
        void setZoom(const qreal current, const qreal previous);
        void setImage(const QImage &img);

    private:
        PictureItem *m_picItem;
        QPixmap m_pixmap;
        QPixmap m_pixmap_edited;

    protected:
        void paintEvent(QPaintEvent *event);

    };

    class PictureItemGL : public QGLWidget
    {
    public:

        explicit PictureItemGL(const QGLFormat& format, PictureItem* parent, const QGLWidget* shareWidget = 0, Qt::WindowFlags f=0);
        ~PictureItemGL();
        void setRotation(const qreal current, const qreal previous);
        void updateClearColor();
        void setImage(const QImage &img);
        void setTexture(const QImage &tex, const int num);
        void textureLoadFinished();

    private:
        void updateSize();
        void clearTextures();
        PictureItem *m_picItem;

        qreal m_scaleX;
        qreal m_scaleY;
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
    void ScrollPageHorizontal(const int value);
    void ScrollPageVertical(const int value);
    void calculateAverageColor(const QImage &img);

    bool m_pixmapNull;
    bool m_flag_jumpToEnd;

    QList<qreal> m_defaultZoomSizes;
    qreal m_zoom_value;
    qreal m_rotation_value;
    LockMode::Mode m_lockMode;
    QColor m_color_clear;

    Settings *m_settings;
    bool m_opengl;
    int m_returnTexCount;
    PictureItemRaster *m_imageDisplay_raster;
    PictureItemGL *m_imageDisplay_gl;
    QFutureWatcher< QImage > *m_loader_image;
    QFutureWatcher< QImage > *m_loader_texture;

    QTimer *m_timer_scrollPage;

    QPoint pointToOrigin(const int width, const int height);
    void avoidOutOfScreen();
    void drag(const QPoint &pt);
    void beginDrag(const QPoint &pt);
    void endDrag();
    void updateLockMode();
    void setPixmapNull(const bool value);

    bool m_dragging;
    QRectF m_boundingRect;
    qreal m_offsetX;
    qreal m_offsetY;
    QPoint m_point_drag;

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
    void quit();
    void boss();

public slots:
    void zoomIn();
    void zoomOut();
    void fitToScreen();
    void fitWidth();
    void fitHeight();
    void updateSize();

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
