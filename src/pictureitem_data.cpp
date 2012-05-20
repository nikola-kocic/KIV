#include "pictureitem_data.h"

PictureItemData::PictureItemData()
    : m_boundingRect(QRectF())
    , m_flag_jumpToEnd(false)
    , m_color_clear(Qt::lightGray)
    , m_offsetX(0)
    , m_offsetY(0)
    , m_pixmapNull(true)
    , m_zoom_value(1.0)
    , m_rotation_value(0.0)
    , m_lockMode(LockMode::None)

{
}

void PictureItemData::setZoom(const qreal z)
{
    if (z < 0.001)
    {
        m_zoom_value = 0.001;
    }
    else if (z > 1000)
    {
        m_zoom_value = 1000;
    }
    else
    {
        m_zoom_value = z;
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
        if (m_zoom_value == 1)
        {
            m_offsetX = qRound((widgetSize.width() - m_boundingRect.width()) / 2);
        }
        else
        {
            m_offsetX = (widgetSize.width() - m_boundingRect.width()) / 2;
        }
    }
    else
    {
        m_offsetX = 0;
    }

    if (widgetSize.height() > m_boundingRect.height())
    {
        if (m_zoom_value == 1)
        {
            m_offsetY = qRound((widgetSize.height() - m_boundingRect.height()) / 2);
        }
        else
        {
            m_offsetY = (widgetSize.height() - m_boundingRect.height()) / 2;
        }
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
    else if ((m_boundingRect.x() <= (m_boundingRect.width() - widgetSize.width()) - ((m_boundingRect.width() - widgetSize.width()) * 2)))
    {
        if ((m_boundingRect.width() - widgetSize.width()) - ((m_boundingRect.width() - widgetSize.width()) * 2) <= 0)
        {
            /* I am too far to the left! */
            m_boundingRect.moveLeft((m_boundingRect.width() - widgetSize.width()) - ((m_boundingRect.width() - widgetSize.width()) * 2));
        }
        else
        {
            /* I am too far to the right! */
            m_boundingRect.moveLeft(0);
        }
    }

    /* Am I lined out to the top? */
    if (m_boundingRect.y() >= 0)
    {
        m_boundingRect.moveTop(0);
    }
    else if ((m_boundingRect.y() <= (m_boundingRect.height() - widgetSize.height()) - ((m_boundingRect.height() - widgetSize.height()) * 2)))
    {
        if ((m_boundingRect.height() - widgetSize.height()) - ((m_boundingRect.height() - widgetSize.height()) * 2) <= 0)
        {
            /* I am too far to the top! */
            m_boundingRect.moveTop((m_boundingRect.height() - widgetSize.height()) - ((m_boundingRect.height() - widgetSize.height()) * 2));
        }
        else
        {
            /* I am too far to the bottom! */
            m_boundingRect.moveTop(0);
        }
    }
}

QPointF PictureItemData::pointToOrigin(const qreal width, const qreal height, const QSize &widgetSize)
{
    const qreal zoomX = (qreal)width / (qreal)m_boundingRect.width();
    const qreal zoomY = (qreal)height / (qreal)m_boundingRect.height();

    if (width > widgetSize.width())
    {
        const qreal oldX = (m_boundingRect.x() - (m_boundingRect.x() * 2)) + (widgetSize.width() / 2);
        const qreal oldY = (m_boundingRect.y() - (m_boundingRect.y() * 2)) + (widgetSize.height() / 2);

        const qreal newX = oldX * zoomX;
        const qreal newY = oldY * zoomY;

        const qreal originX = newX - (widgetSize.width() / 2) - ((newX - (widgetSize.width() / 2)) * 2);
        const qreal originY = newY - (widgetSize.height() / 2) - ((newY - (widgetSize.height() / 2)) * 2);

        return QPointF(originX, originY);
    }
    else
    {
        if (height > widgetSize.height())
        {
            const qreal oldY = (m_boundingRect.y() - (m_boundingRect.y() * 2)) + (widgetSize.height() / 2);

            const qreal newY = oldY * zoomY;

            const qreal originY = newY - (widgetSize.height() / 2) - ((newY - (widgetSize.height() / 2)) * 2);

            return QPointF(0, originY);
        }
        else
        {
            return QPointF(0, 0);
        }
    }
}
