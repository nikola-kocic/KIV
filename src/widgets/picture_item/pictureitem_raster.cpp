#include "widgets/picture_item/pictureitem_raster.h"

#include <QPainter>
#include <QPaintEvent>

#include "picture_loader.h"
#include "settings.h"

//#define DEBUG_PICTUREITEM_RASTER
#ifdef DEBUG_PICTUREITEM_RASTER
#include "helper.h"
#endif

PictureItemRaster::PictureItemRaster(PictureItemData *data, ZoomFilter zoomFilter, QWidget *parent)
    : QWidget(parent)
    , PictureItemInterface(data)
    , m_pixmap(QPixmap())
    , m_pixmap_edited(m_pixmap)
    , m_zoomFilter(zoomFilter)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    m_widget = this;
}

PictureItemRaster::~PictureItemRaster()
{
#ifdef DEBUG_PICTUREITEM_RASTER
    DEBUGOUT;
#endif
}

void PictureItemRaster::setImage(const QImage &img)
{
    this->setUpdatesEnabled(false);

    m_pixmap = QPixmap::fromImage(img);
    m_pixmap_edited = m_pixmap;

    this->setUpdatesEnabled(true);
    this->update();
}

void PictureItemRaster::paint(QPainter &p, QPaintEvent const * const event)
{
    const QRect eventRect = event->rect();
    const bool drawFullBackground = (m_data->isPixmapNull() ||
                                     m_pixmap_edited.hasAlpha());

    if (drawFullBackground)
    {
        p.fillRect(eventRect, m_data->m_color_clear);
    }
    if (! m_data->isPixmapNull())
    {
        const QPointF offset = m_data->getOffset();
        const QRectF bounding_rect = m_data->getBoundingRect();
        const QRectF targetRect = QRectF(
                    offset.x(),
                    offset.y(),
                    qMin(bounding_rect.width(), 1.0 * eventRect.width()),
                    qMin(bounding_rect.height(), 1.0 * eventRect.height())
                    );

        if (! drawFullBackground)
        {
            /* Draw background */
            const QRegion eventRegion = event->region();
            const QRegion backgroundRegion = eventRegion.subtracted(
                        QRegion(targetRect.toRect().adjusted(1, 1, -1, -1)));
            for (const QRect &backgroundRect : backgroundRegion)
            {
                p.fillRect(backgroundRect, m_data->m_color_clear);
            }
        }

        /* Calculate source rect */
        const qreal zoom = m_data->getZoom();
        const QRectF sourceRect = QRectF(-bounding_rect.topLeft() / zoom,
                                         targetRect.size() / zoom);

        /* Draw image */
        p.setClipRect(eventRect);
        if (m_zoomFilter == ZoomFilter::Good)
        {
            p.setRenderHint(QPainter::SmoothPixmapTransform);
        }
        p.drawPixmap(targetRect, m_pixmap_edited, sourceRect);
    }
}

void PictureItemRaster::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    this->paint(p, event);
    p.end();
}

void PictureItemRaster::setRotation(const qreal current, const qreal previous)
{
    const QSizeF image_size_transformed = m_data->getImageSizeTransformed();
    if (qFuzzyIsNull(m_data->getRotation()))
    {
        m_pixmap_edited = m_pixmap;
    }
    else if (!Helper::FuzzyCompare(current, previous))
    {
        m_pixmap_edited = QPixmap(image_size_transformed.toSize());

        m_pixmap_edited.fill(m_data->m_color_clear);
        QPainter p(&m_pixmap_edited);

        if (m_zoomFilter == ZoomFilter::Good)
        {
            p.setRenderHint(QPainter::SmoothPixmapTransform);
        }
        p.translate(image_size_transformed.width() / 2,
                    image_size_transformed.height() / 2);
        p.rotate(current);
        p.translate(-image_size_transformed.width() / 2,
                    -image_size_transformed.height() / 2);
        p.drawPixmap(
            QPointF((image_size_transformed.width() - m_pixmap.width()) / 2,
                    (image_size_transformed.height() - m_pixmap.height()) / 2),
            m_pixmap);

        p.end();
    }

    this->update();
}

void PictureItemRaster::setZoom(const qreal, const qreal)
{
    this->update();
}
