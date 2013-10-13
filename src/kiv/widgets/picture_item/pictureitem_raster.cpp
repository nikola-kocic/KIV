#include "widgets/picture_item/pictureitem_raster.h"

#include <QPainter>
#include <QPaintEvent>

#include "picture_loader.h"
#include "settings.h"

//#define DEBUG_PICTUREITEM_RASTER
#ifdef DEBUG_PICTUREITEM_RASTER
#include "helper.h"
#endif

PictureItemRaster::PictureItemRaster(PictureItemData *data, QWidget *parent)
    : QWidget(parent)
    , PictureItemInterface(data, parent)
    , m_pixmap(QPixmap())
    , m_pixmap_edited(m_pixmap)
{
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

void PictureItemRaster::paintEvent(QPaintEvent *event)
{
    if (m_data->isPixmapNull())
    {
        return;
    }

    const QRect boundingRect = event->rect();
    QPainter p(this);
    p.setClipRect(boundingRect);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    p.fillRect(boundingRect, m_data->m_color_clear);
    const qreal zoom = m_data->getZoom();
    const QPointF offset = m_data->getOffset();
    const QRectF sourceRect = QRectF(-m_data->m_boundingRect.x() / zoom,
                                     -m_data->m_boundingRect.y() / zoom,
                                     boundingRect.width() / zoom,
                                     boundingRect.height() / zoom);
    const QRectF drawRect = QRectF(offset, boundingRect.size());
    p.drawPixmap(drawRect, m_pixmap_edited, sourceRect);
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

        p.setRenderHint(QPainter::SmoothPixmapTransform);
        p.translate(image_size_transformed.width() / 2,
                    image_size_transformed.height() / 2);
        p.rotate(current);
        p.translate(-image_size_transformed.width() / 2,
                    -image_size_transformed.height() / 2);
        p.drawPixmap((image_size_transformed.width() - m_pixmap.width()) / 2,
                     (image_size_transformed.height() - m_pixmap.height()) / 2,
                     m_pixmap);
        p.end();
    }

    const QSizeF newSize = image_size_transformed * m_data->getZoom();
    const QPointF p = m_data->pointToOrigin();
    m_data->m_boundingRect = QRectF(p, newSize);
    m_data->avoidOutOfScreen();
    m_data->updateOffsets();
    this->update();
}

void PictureItemRaster::setZoom(const qreal current, const qreal /*previous*/)
{
    const QSizeF newSize = m_data->getImageSizeTransformed() * current;
    const QPointF p = m_data->pointToOrigin();
    m_data->m_boundingRect = QRectF(p, newSize);
    m_data->avoidOutOfScreen();
    m_data->updateOffsets();
    this->update();
}
