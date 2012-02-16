#ifndef PICTUREITEM_H
#define PICTUREITEM_H

#include "enums.h"
#include "settings.h"
#include "picture_loader.h"
#include "pictureitem_gl.h"
#include "pictureitem_raster.h"
#include "teximg.h"

#include <QBoxLayout>
#include <QFutureWatcher>
#include <QTimer>
#include <QtConcurrentMap>
#include <QtConcurrentRun>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

//#define DEBUG_PICTUREITEM

#ifdef DEBUG_PICTUREITEM
    #include <QDateTime>
#endif

class PictureItemRaster;
class PictureItemGL;

class PictureItem : public QWidget
{
    Q_OBJECT

    friend class PictureItemRaster;
    friend class PictureItemGL;

public:
    explicit PictureItem(Settings *settings, QWidget *parent = 0, Qt::WindowFlags f = 0);

    void setZoom(const qreal z);
    qreal getZoom() const;

    void setRotation(const qreal r);
    qreal getRotation() const;

    void setLockMode(const int mode);
    int getLockMode() const;

    void setHardwareAcceleration(const bool b);
    bool getHardwareAcceleration() const;

    void setPixmap(const FileInfo &info);
    bool isPixmapNull() const;


private:
    void initPictureItem();
    void start_timerScrollPage();
    void afterPixmapLoad();
    void ScrollPageHorizontal(const int value);
    void ScrollPageVertical(const int value);
    void calculateAverageColor(const QImage &img);

    bool m_pixmapNull;
    bool m_flag_jumpToEnd;

    qreal m_zoom_value;
    qreal m_rotation_value;
    int m_lockMode;
    QColor m_color_clear;

    Settings *m_settings;
    bool m_opengl;
    PictureItemRaster *m_imageDisplay_raster;
    PictureItemGL *m_imageDisplay_gl;
    QFutureWatcher< QImage > *m_loader_image;

    QTimer *m_timer_scrollPage;

    QPointF pointToOrigin(const qreal width, const qreal height);
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
    void imageFinished(int num);

protected:
    void wheelEvent(QWheelEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *);
};

#endif // PICTUREITEM_H
