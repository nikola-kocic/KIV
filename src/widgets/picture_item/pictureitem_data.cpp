#include "widgets/picture_item/pictureitem_data.h"
#include <QTransform>
#include <QtDebug>

PictureItemData::PictureItemData()
    : m_color_clear(Qt::lightGray)
    , m_boundingRect(QRectF())
    , m_offset(QPoint(0, 0))
    , m_zoom_value(1.0)
    , m_rotation_value(0.0)
    , m_point_drag(QPointF())
    , m_pixmapNull(true)
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
    updateSize();
    return 0;
}

void PictureItemData::setRotation(const qreal r)
{
    if (qRound(r) % 360 == 0)
    {
        m_img_size_transformed = m_img_size_original;
        m_rotation_value = 0;
    }
    else
    {
        QTransform tRot;
        tRot.rotate(r);
        const QRectF transformedRect = tRot.mapRect(
                    QRectF(QPointF(0, 0), m_img_size_original));
        m_img_size_transformed = transformedRect.size();
        m_rotation_value = r;
    }
    updateSize();
}

/* Calculates zoom value needed for picture to fit to widget size.
 * If picture is smaller than widget, it's set to 100% zoom.
 * zoomVal: stores calculated zoom value.
 * return: 0 if successful.
 */
int PictureItemData::fitToScreen(qreal &zoomVal) const
{
    if (this->isPixmapNull())
    {
        return 1;
    }

    const QSizeF orig_size = m_boundingRect.size() / m_zoom_value;

    const qreal x_ratio = 1.0 * m_widget_size.width() / orig_size.width();
    const qreal y_ratio = 1.0 * m_widget_size.height() / orig_size.height();

    if ((orig_size.width() <= m_widget_size.width())
        && (orig_size.height() <= m_widget_size.height()))
    {
        zoomVal = 1;
    }
    else if ((x_ratio * orig_size.height()) < m_widget_size.height())
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
 * zoomVal: stores calculated zoom value.
 * return: 0 if successful.
 */
int PictureItemData::fitWidth(qreal &zoomVal) const
{
    if (this->isPixmapNull())
    {
        return 1;
    }

    const qreal tw = m_boundingRect.width() / m_zoom_value;

    const qreal x_ratio = 1.0 * m_widget_size.width() / tw;

    if (tw <= m_widget_size.width())
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
 * zoomVal: stores calculated zoom value.
 * return: 0 if successful.
 */
int PictureItemData::fitHeight(qreal &zoomVal) const
{
    if (this->isPixmapNull())
    {
        return 1;
    }

    const qreal th = m_boundingRect.height() / m_zoom_value;

    const qreal y_ratio = 1.0 * m_widget_size.height() / th;

    if (th <= m_widget_size.height())
    {
        zoomVal = 1;
    }
    else
    {
        zoomVal = y_ratio;
    }
    return 0;
}

void PictureItemData::updateOffsets()
{
    if (this->isPixmapNull())
    {
        return;
    }

    if (m_widget_size.width() > m_boundingRect.width())
    {
        const qreal offsetX = (m_widget_size.width() - m_boundingRect.width()) / 2;
        m_offset.setX(qFuzzyCompare(m_zoom_value, 1.0) ? qRound(offsetX) : offsetX);
    }
    else
    {
        m_offset.setX(0);
    }

    if (m_widget_size.height() > m_boundingRect.height())
    {
        const qreal offsetY =
                (m_widget_size.height() - m_boundingRect.height()) / 2;
        m_offset.setY(qFuzzyCompare(m_zoom_value, 1.0) ? qRound(offsetY) : offsetY);
    }
    else
    {
        m_offset.setY(0);
    }
}

void PictureItemData::avoidOutOfScreen()
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
        const qreal widthDiff = m_widget_size.width() - m_boundingRect.width();
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
        const qreal heightDiff = m_widget_size.height() - m_boundingRect.height();
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

QPointF PictureItemData::pointToOrigin() const
{
    QSizeF img_size_zoomed = m_img_size_transformed * m_zoom_value;
    qreal originX = 0;
    qreal originY = 0;

    if (img_size_zoomed.width() > m_widget_size.width())
    {
        if (m_boundingRect.width() < m_widget_size.width())
        {
            originX = (m_widget_size.width() - img_size_zoomed.width()) / 2;
        }
        else
        {
            const qreal zoomX = img_size_zoomed.width() / m_boundingRect.width();
            const qreal oldX = (m_widget_size.width() / 2) - m_boundingRect.x();
            const qreal newX = oldX * zoomX;
            originX = (m_widget_size.width() / 2) - newX;
        }
    }

    if (img_size_zoomed.width() > m_widget_size.width()
        || img_size_zoomed.height() > m_widget_size.height())
    {
        if (m_boundingRect.height() < m_widget_size.height())
        {
            originY = (m_widget_size.height() - img_size_zoomed.height()) / 2;
        }
        else
        {
            const qreal zoomY = img_size_zoomed.height() / m_boundingRect.height();
            const qreal oldY = (m_widget_size.height() / 2) - m_boundingRect.y();
            const qreal newY = oldY * zoomY;
            originY = (m_widget_size.height() / 2) - newY;
        }
    }

    return QPointF(originX, originY);
}

void PictureItemData::updateSize()
{
    const QSizeF img_size_zoomed_transformed = (m_img_size_transformed *
                                                m_zoom_value);
    const QPointF p = pointToOrigin();
    m_boundingRect = QRectF(p, img_size_zoomed_transformed);
    avoidOutOfScreen();
    updateOffsets();
}

int PictureItemData::resetImagePosition(bool rtl)
{
    if (m_boundingRect.width() > m_widget_size.width())
    {
        if (rtl == true)
        {
            m_boundingRect.moveLeft(
                        -(m_boundingRect.width() - m_widget_size.width()));
        }
        else
        {
            m_boundingRect.moveLeft(0);
        }
    }

     m_boundingRect.moveTop(0);
     return 0;
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

void PictureItemData::drag(const QPoint &pt)
{
    if (this->isPixmapNull())
    {
        return;
    }

    const qreal widthDiff = m_widget_size.width() - m_boundingRect.width();
    const qreal xDiff = pt.x() - m_point_drag.x();

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

    const qreal heightDiff = m_widget_size.height() - m_boundingRect.height();
    const qreal yDiff = pt.y() - m_point_drag.y();

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
