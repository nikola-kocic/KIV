#include "pictureitem.h"
#include "settings.h"
#include "picture_loader.h"

//#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

PictureItemRaster::PictureItemRaster(PictureItem *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_picItem(parent)
    , m_pixmap(QPixmap())
    , m_pixmap_edited(m_pixmap)
{
}

void PictureItemRaster::setImage(const QImage &img)
{
    this->setUpdatesEnabled(false);

    m_pixmap = QPixmap::fromImage(img);
    m_img_size = m_pixmap.size();

    m_pixmap_edited = m_pixmap;

    m_picItem->m_boundingRect = QRect(0, 0, m_pixmap.width(), m_pixmap.height());
    if (m_picItem->getLockMode() == LockMode::None)
    {
        m_picItem->setZoom(1);
    }
    m_picItem->setRotation(0);

    m_picItem->afterPixmapLoad();
    m_picItem->updateSize();

    this->setUpdatesEnabled(true);
    this->update();
}

void PictureItemRaster::paintEvent(QPaintEvent *event)
{
    if (m_picItem->isPixmapNull())
    {
        return;
    }

    const QRect boundingRect = event->rect();
    QPainter p(this);
    p.setClipRect(boundingRect);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    p.fillRect(boundingRect, m_picItem->m_color_clear);
    const qreal zoom = m_picItem->getZoom();
    const QRectF sourceRect = QRectF(-m_picItem->m_boundingRect.x() / zoom,
                               -m_picItem->m_boundingRect.y() / zoom,
                               boundingRect.width() / zoom,
                               boundingRect.height() / zoom);
    const QRectF drawRect = QRectF(QPointF(m_picItem->m_offsetX, m_picItem->m_offsetY), boundingRect.size());
    p.drawPixmap(drawRect, m_pixmap_edited, sourceRect);
    p.end();
}

void PictureItemRaster::setRotation(const qreal current, const qreal previous)
{
    if (qRound(current) % 360 == 0)
    {
        m_img_size = m_pixmap.size();
        m_pixmap_edited = m_pixmap;
    }
    else if (!Helper::FuzzyCompare(current, previous))
    {
        QTransform tRot;
        tRot.rotate(current);
        const QRect transformedRect = tRot.mapRect(QRect(QPoint(0, 0), m_pixmap.size()));

        m_pixmap_edited = QPixmap(transformedRect.width(), transformedRect.height());
        m_img_size = m_pixmap_edited.size();

        m_pixmap_edited.fill(m_picItem->m_color_clear);
        QPainter p(&m_pixmap_edited);

        p.setRenderHint(QPainter::SmoothPixmapTransform);
        p.translate(transformedRect.width() / 2, transformedRect.height() / 2);
        p.rotate(current);
        p.translate(-transformedRect.width() / 2, -transformedRect.height() / 2);
        p.drawPixmap((transformedRect.width() - m_pixmap.width()) / 2, (transformedRect.height() - m_pixmap.height()) / 2, m_pixmap);
        p.end();
    }

    const QPointF p = m_picItem->pointToOrigin((m_img_size.width() * m_picItem->getZoom()), (m_img_size.height() * m_picItem->getZoom()));
    m_picItem->m_boundingRect = QRectF(p.x(), p.y(), (m_img_size.width() * m_picItem->getZoom()), (m_img_size.height() * m_picItem->getZoom()));

    m_picItem->avoidOutOfScreen();
    m_picItem->updateSize();
    this->update();
}

void PictureItemRaster::setZoom(const qreal current, const qreal previous)
{
    Q_UNUSED(previous);
    const QPointF p = m_picItem->pointToOrigin((m_img_size.width() * current), (m_img_size.height() * current));

    m_picItem->m_boundingRect = QRectF(p.x(), p.y(), (m_img_size.width() * current), (m_img_size.height() * current));

    m_picItem->avoidOutOfScreen();
    m_picItem->updateSize();
    this->update();
}
