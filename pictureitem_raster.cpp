#include "pictureitem.h"
#include "settings.h"
#include "picture_loader.h"

//#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

PictureItem::PictureItemRaster::PictureItemRaster(PictureItem *parent, Qt::WindowFlags f) : QWidget(parent)
{
    m_picItem = parent;
}

void PictureItem::PictureItemRaster::setImage(QImage img)
{
    this->setUpdatesEnabled(false);

    m_pixmap = QPixmap::fromImage(img);

    m_pixmap_edited = m_pixmap;

    m_picItem->m_boundingRect = QRect(0, 0, m_pixmap.width(), m_pixmap.height());
    if (m_picItem->getLockMode() != LockMode::Zoom)
    {
        m_picItem->setZoom(1);
    }
    m_picItem->setRotation(0);

    m_picItem->afterPixmapLoad();

    this->setUpdatesEnabled(true);
    this->update();
}

void PictureItem::PictureItemRaster::paintEvent(QPaintEvent *event)
{
    if (m_picItem->isPixmapNull())
    {
        return;
    }

    QPainter p(this);
    p.setClipRect(event->region().boundingRect());
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    p.fillRect(event->region().boundingRect(), m_picItem->m_color_clear);
    qreal zoom = m_picItem->getZoom();
    QRectF sourceRect = QRectF(-m_picItem->m_boundingRect.x() / zoom,
                               -m_picItem->m_boundingRect.y() / zoom,
                               event->region().boundingRect().width() / zoom,
                               event->region().boundingRect().height() / zoom);

    QRectF drawRect = (QRectF)event->region().boundingRect();

    if (drawRect.width() > (m_pixmap_edited.width() * m_picItem->getZoom()))
    {
        drawRect.moveLeft((drawRect.width() - (m_pixmap_edited.width() * zoom)) / 2);
    }
    if (drawRect.height() > (m_pixmap_edited.height() * m_picItem->getZoom()))
    {
        drawRect.moveTop((drawRect.height() - (m_pixmap_edited.height() * zoom)) / 2);
    }

    p.drawPixmap(drawRect, m_pixmap_edited, sourceRect);
    p.end();
}

void PictureItem::PictureItemRaster::setRotation(qreal r)
{
    if ((int)m_picItem->getRotation() % 360 == 0)
    {
        m_pixmap_edited = m_pixmap;
    }
    else
    {
        QTransform tRot;
        tRot.rotate(m_picItem->getRotation());

        Qt::TransformationMode rotateMode;
        rotateMode = Qt::SmoothTransformation;
    //    rotateMode = Qt::FastTransformation;

        m_pixmap_edited = m_pixmap.transformed(tRot, rotateMode);
    }

    m_picItem->m_boundingRect.setWidth(m_pixmap_edited.width() * m_picItem->getZoom());
    m_picItem->m_boundingRect.setHeight(m_pixmap_edited.height() * m_picItem->getZoom());
    m_picItem->avoidOutOfScreen();
    this->update();
}

void PictureItem::PictureItemRaster::setZoom(qreal current, qreal previous)
{
    QPointF p = m_picItem->pointToOrigin((m_pixmap_edited.width() * current), (m_pixmap_edited.height() * current));

    m_picItem->m_boundingRect = QRectF(p.x(), p.y(), (m_pixmap_edited.width() * current), (m_pixmap_edited.height() * current));

    m_picItem->avoidOutOfScreen();
    this->update();
}
