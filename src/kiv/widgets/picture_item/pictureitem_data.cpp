#include "pictureitem_data.h"

PictureItemData::PictureItemData()
    : m_boundingRect(QRectF())
    , m_color_clear(Qt::lightGray)
    , m_offsetX(0)
    , m_offsetY(0)
    , m_pixmapNull(true)
    , m_zoom_value(1.0)
    , m_rotation_value(0.0)
    , m_lockMode(LockMode::None)
{
}

/* Sets zoom value in range of 0.001 to 1000. If it's not within that range,
 * it's rounded to nearest valid value.
 * desiredZoom: zoom that should be applied.
 * zoomVal: stores applied zoom value.
 * return: 0 if successful.
 */
int PictureItemData::setZoom(const qreal desiredZoom, qreal &zoomVal)
{
    m_zoom_value = (desiredZoom < 0.001) ? 0.001 :
                                 (desiredZoom > 1000) ? 1000 :
                                              desiredZoom;
    zoomVal = m_zoom_value;
    return 0;
}
}

void PictureItemData::updateSize(const QSize &widgetSize)
{
    if (this->isPixmapNull())
    {
        return;
    }

    if (widgetSize.width() > m_boundingRect.width())
    {
        const qreal offsetX = (widgetSize.width() - m_boundingRect.width()) / 2;
        m_offsetX = (m_zoom_value == 1) ? qRound(offsetX) : offsetX;
    }
    else
    {
        m_offsetX = 0;
    }

    if (widgetSize.height() > m_boundingRect.height())
    {
        const qreal offsetY =
                (widgetSize.height() - m_boundingRect.height()) / 2;
        m_offsetY = (m_zoom_value == 1) ? qRound(offsetY) : offsetY;
    }
    else
    {
        m_offsetY = 0;
    }
}

void PictureItemData::avoidOutOfScreen(const QSize &widgetSize)
{
    if (this->isPixmapNull())
    {
        return;
    }

    /* Am I lined out to the left? */
    if (m_boundingRect.x() >= 0)
    {
        m_boundingRect.moveLeft(0);
    }
    else
    {
        const qreal widthDiff = widgetSize.width() - m_boundingRect.width();
        if ((m_boundingRect.x() <= widthDiff))
        {
            if (widthDiff <= 0)
            {
                /* I am too far to the left! */
                m_boundingRect.moveLeft(widthDiff);
            }
            else
            {
                /* I am too far to the right! */
                m_boundingRect.moveLeft(0);
            }
        }
    }

    /* Am I lined out to the top? */
    if (m_boundingRect.y() >= 0)
    {
        m_boundingRect.moveTop(0);
    }
    else
    {
        const qreal heightDiff = widgetSize.height() - m_boundingRect.height();
        if ((m_boundingRect.y() <= heightDiff))
        {
            if (heightDiff <= 0)
            {
                /* I am too far to the top! */
                m_boundingRect.moveTop(heightDiff);
            }
            else
            {
                /* I am too far to the bottom! */
                m_boundingRect.moveTop(0);
            }
        }
    }
}

QPointF PictureItemData::pointToOrigin(const qreal width, const qreal height,
                                       const QSize &widgetSize)
{
    qreal originX = 0;
    qreal originY = 0;

    if (width > widgetSize.width())
    {
        const qreal zoomX = (qreal)width / (qreal)m_boundingRect.width();
        const qreal oldX = (widgetSize.width() / 2) - m_boundingRect.x();
        const qreal newX = oldX * zoomX;
        originX = (widgetSize.width() / 2) - newX;
    }

    if (width > widgetSize.width() || height > widgetSize.height())
    {
        const qreal zoomY = (qreal)height / (qreal)m_boundingRect.height();
        const qreal oldY = (widgetSize.height() / 2) - m_boundingRect.y();
        const qreal newY = oldY * zoomY;
        originY = (widgetSize.height() / 2) - newY;
    }

    return QPointF(originX, originY);
}
