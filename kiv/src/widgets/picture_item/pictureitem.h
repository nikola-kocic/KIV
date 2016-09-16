#ifndef PICTUREITEM_H
#define PICTUREITEM_H

#include <QBoxLayout>
#include <QFutureWatcher>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QtConcurrent/QtConcurrentMap>
#include <QtConcurrent/QtConcurrentRun>
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
    explicit PictureItem(const IPictureLoader *const picture_loader,
                         Settings const * const settings,
                         QWidget *parent = nullptr,
                         Qt::WindowFlags f = nullptr);
    ~PictureItem() override;

    qreal getZoom() const;

    void setRotation(const qreal value);
    qreal getRotation() const;

    void setLockMode(const LockMode mode);

    void initPictureItem(bool opengl);

    void setPixmap(const FileInfo &info);
    bool isPixmapNull() const;


private:
    void afterPixmapLoad();
    QColor getAverageColor(const QImage &img) const;

    void drag(const QPoint &pt);
    void beginDrag(const QPoint &pt);
    void endDrag();
    void updateLockMode();

    const IPictureLoader *const m_picture_loader;
    PictureItemData *const m_data;
    const Settings *const m_settings;
    PictureItemInterface *m_imageDisplay;
    QFutureWatcher< QImage > *const m_loader_image;
    LockMode m_lockMode;

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
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

inline bool PictureItem::isPixmapNull() const
{ return m_data->isPixmapNull(); }

inline qreal PictureItem::getRotation() const
{ return m_data->getRotation(); }

inline qreal PictureItem::getZoom() const
{ return m_data->getZoom(); }


#endif  // PICTUREITEM_H
