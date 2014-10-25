#ifndef PICTUREITEM_H
#define PICTUREITEM_H

#include <QBoxLayout>
#include <QFutureWatcher>
#include <QKeyEvent>
#include <QMouseEvent>
#ifdef QT5
#include <QtConcurrent/QtConcurrentMap>
#include <QtConcurrent/QtConcurrentRun>
#else
#include <QtConcurrentMap>
#include <QtConcurrentRun>
#endif
#include <QTimer>
#include <QWheelEvent>

#include "kiv/include/IPictureLoader.h"
#include "kiv/src/settings.h"
#include "kiv/src/widgets/picture_item/pictureitem_data.h"
#include "kiv/src/widgets/picture_item/pictureitem_gl.h"
#include "kiv/src/widgets/picture_item/pictureitem_raster.h"

//#define DEBUG_PICTUREITEM
#ifdef DEBUG_PICTUREITEM
    #include "kiv/src/helper.h"
#endif

class PictureItem : public QWidget
{
    Q_OBJECT

public:
    explicit PictureItem(IPictureLoader *picture_loader,
                         Settings const * const settings,
                         QWidget *parent = 0,
                         Qt::WindowFlags f = 0);
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
    void afterPixmapLoad();
    QColor getAverageColor(const QImage &img) const;

    void drag(const QPoint &pt);
    void beginDrag(const QPoint &pt);
    void endDrag();
    void updateLockMode();

    IPictureLoader *m_picture_loader;
    PictureItemData *m_data;
    const Settings *m_settings;
    bool m_opengl;
    PictureItemInterface *m_imageDisplay;
    QFutureWatcher< QImage > *m_loader_image;
    int m_lockMode;

    bool m_dragging;

#ifdef DEBUG_PICTUREITEM
    QTime t;
#endif

signals:
    void zoomChanged(qreal current, qreal previous);
    void imageChanged();

public slots:
    void setZoom(const qreal value);
    void fitToScreen();
    void fitWidth();
    void fitHeight();
    void scrollPageHorizontal(const int value);
    void scrollPageVertical(const int value);

private slots:
    void imageFinished(int num);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

inline bool PictureItem::getHardwareAcceleration() const
{ return m_opengl; }

inline bool PictureItem::isPixmapNull() const
{ return m_data->isPixmapNull(); }

inline qreal PictureItem::getRotation() const
{ return m_data->getRotation(); }

inline qreal PictureItem::getZoom() const
{ return m_data->getZoom(); }


#endif  // PICTUREITEM_H
