#ifndef PICTUREITEM_H
#define PICTUREITEM_H

#include "settings.h"
#include "picture_loader.h"
#include "pictureitem_data.h"
#include "pictureitem_gl.h"
#include "pictureitem_raster.h"
#include "teximg.h"

#include <QBoxLayout>
#include <QFutureWatcher>
#include <QTimer>
#ifdef QT5
#include <QtConcurrent/QtConcurrentMap>
#include <QtConcurrent/QtConcurrentRun>
#else
#include <QtConcurrentMap>
#include <QtConcurrentRun>
#endif

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

//#define DEBUG_PICTUREITEM

#ifdef DEBUG_PICTUREITEM
    #include <QDateTime>
#endif

class PictureItem : public QWidget
{
    Q_OBJECT

public:
    explicit PictureItem(Settings *settings, QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~PictureItem();

    qreal getZoom() const;

    void setRotation(const qreal value);
    qreal getRotation() const;

    void setLockMode(const int mode);

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

    void drag(const QPoint &pt);
    void beginDrag(const QPoint &pt);
    void endDrag();
    void updateLockMode();

    PictureItemData *m_data;
    Settings *m_settings;
    bool m_opengl;
    PictureItemInterface *m_imageDisplay;
    QFutureWatcher< QImage > *m_loader_image;

    QTimer *m_timer_scrollPage;

    bool m_dragging;
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
    void setZoom(const qreal value);
    void zoomIn();
    void zoomOut();
    void fitToScreen();
    void fitWidth();
    void fitHeight();

private slots:
    void imageFinished(int num);

protected:
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

inline bool PictureItem::getHardwareAcceleration() const
{
    return m_opengl;
}

inline bool PictureItem::isPixmapNull() const
{
    return m_data->isPixmapNull();
}

inline qreal PictureItem::getRotation() const
{
    return m_data->getRotation();
}

inline qreal PictureItem::getZoom() const
{
    return m_data->getZoom();
}


#endif // PICTUREITEM_H
