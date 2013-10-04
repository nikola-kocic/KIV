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
    , m_point_drag(QPoint())
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

/* Calculates zoom value needed for picture to fit to widget size.
 * If picture is smaller than widget, it's set to 100% zoom.
 * widgetSize: size of widget containing picture.
 * zoomVal: stores calculated zoom value.
 * return: 0 if successful.
 */
int PictureItemData::fitToScreen(const QSize &widgetSize, qreal &zoomVal) const
{
    if (this->isPixmapNull())
    {
        return 1;
    }

    const QSize orig_size = QSize(
                m_boundingRect.width() / m_zoom_value,
                m_boundingRect.height() / m_zoom_value);

    const qreal x_ratio = (qreal)widgetSize.width() / orig_size.width();
    const qreal y_ratio = (qreal)widgetSize.height() / orig_size.height();

    if ((orig_size.width() <= widgetSize.width())
        && (orig_size.height() <= widgetSize.height()))
    {
        zoomVal = 1;
    }
    else if ((x_ratio * orig_size.height()) < widgetSize.height())
    {
        zoomVal = x_ratio;
    }
    else
    {
        zoomVal = y_ratio;
    }
    return 0;
}

/* Calculates zoom value needed for picture to fit to widget width.
 * widgetSize: size of widget containing picture.
 * zoomVal: stores calculated zoom value.
 * return: 0 if successful.
 */
int PictureItemData::fitWidth(const QSize &widgetSize, qreal &zoomVal) const
{
    if (this->isPixmapNull())
    {
        return 1;
    }

    const qreal tw = m_boundingRect.width() / m_zoom_value;

    const qreal x_ratio = (qreal)widgetSize.width() / tw;

    if (tw <= widgetSize.width())
    {
        zoomVal = 1;
    }
    else
    {
        zoomVal = x_ratio;
    }
    return 0;
}

/* Calculates zoom value needed for picture to fit to widget height.
 * widgetSize: size of widget containing picture.
 * zoomVal: stores calculated zoom value.
 * return: 0 if successful.
 */
int PictureItemData::fitHeight(const QSize &widgetSize, qreal &zoomVal) const
{
    if (this->isPixmapNull())
    {
        return 1;
    }

    const qreal th = m_boundingRect.height() / m_zoom_value;

    const qreal y_ratio = (qreal)widgetSize.height() / th;

    if (th <= widgetSize.height())
    {
        zoomVal = 1;
    }
    else
    {
        zoomVal = y_ratio;
    }
    return 0;
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

QPointF PictureItemData::pointToOrigin(const QSizeF &pictureSize,
                                       const QSize &widgetSize) const
{
    qreal originX = 0;
    qreal originY = 0;

    if (pictureSize.width() > widgetSize.width())
    {
        const qreal zoomX = pictureSize.width() / m_boundingRect.width();
        const qreal oldX = (widgetSize.width() / 2) - m_boundingRect.x();
        const qreal newX = oldX * zoomX;
        originX = (widgetSize.width() / 2) - newX;
    }

    if (pictureSize.width() > widgetSize.width()
        || pictureSize.height() > widgetSize.height())
    {
        const qreal zoomY = pictureSize.height() / m_boundingRect.height();
        const qreal oldY = (widgetSize.height() / 2) - m_boundingRect.y();
        const qreal newY = oldY * zoomY;
        originY = (widgetSize.height() / 2) - newY;
    }

    return QPointF(originX, originY);
}

void PictureItemData::beginDrag(const QPoint &pt)
{
    if (this->isPixmapNull())
    {
        return;
    }

    /* Initial drag position */
    m_point_drag.setX(pt.x() - m_boundingRect.x());
    m_point_drag.setY(pt.y() - m_boundingRect.y());
}

void PictureItemData::drag(const QPoint &pt,
                           const QSize &widgetSize)
{
    if (this->isPixmapNull())
    {
        return;
    }

    const qreal widthDiff = widgetSize.width() - m_boundingRect.width();
    const int xDiff = pt.x() - m_point_drag.x();

    /* Am I dragging it outside of the panel? */
    if ((xDiff >= widthDiff) && (xDiff <= 0))
    {
        /* No, everything is just fine */
        m_boundingRect.moveLeft(xDiff);
    }
    else if (xDiff > 0)
    {
        /* Now don't drag it out of the panel please */
        m_boundingRect.moveLeft(0);
    }
    else if (xDiff < widthDiff)
    {
        /* I am dragging it out of my panel.
             * How many pixels do I have left? */
        if (widthDiff <= 0)
        {
            /* Make it fit perfectly */
            m_boundingRect.moveLeft(widthDiff);
        }
    }

    const qreal heightDiff = widgetSize.height() - m_boundingRect.height();
    const int yDiff = pt.y() - m_point_drag.y();

    /* Am I dragging it outside of the panel? */
    if (yDiff >= heightDiff && (yDiff <= 0))
    {
        /* No, everything is just fine */
        m_boundingRect.moveTop(yDiff);
    }
    else if (yDiff > 0)
    {
        /* Now don't drag it out of the panel please */
        m_boundingRect.moveTop(0);
    }
    else if (yDiff < heightDiff)
    {
        /* I am dragging it out of my panel.
             * How many pixels do I have left? */
        if (heightDiff <= 0)
        {
            /* Make it fit perfectly */
            m_boundingRect.moveTop(heightDiff);
        }
    }
}
