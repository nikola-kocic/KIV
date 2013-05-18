#include "widgets/picture_item/pictureitem_raster.h"

#include <QPainter>
#include <QPaintEvent>

//#define DEBUG_PICTUREITEM_RASTER

#ifdef DEBUG_PICTUREITEM_RASTER
#include <QDebug>
#include <QDateTime>
#endif

#include "picture_loader.h"
#include "settings.h"

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
#ifdef DEBUG_PICTUREITEM_GL
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "~PictureItemRaster()";
#endif
}

void PictureItemRaster::setImage(const QImage &img)
{
    this->setUpdatesEnabled(false);

    m_pixmap = QPixmap::fromImage(img);
    m_img_size = m_pixmap.size();

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
    const QRectF sourceRect = QRectF(-m_data->m_boundingRect.x() / zoom,
                                     -m_data->m_boundingRect.y() / zoom,
                                     boundingRect.width() / zoom,
                                     boundingRect.height() / zoom);
    const QRectF drawRect = QRectF(QPointF(m_data->m_offsetX, m_data->m_offsetY), boundingRect.size());
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

        m_pixmap_edited.fill(m_data->m_color_clear);
        QPainter p(&m_pixmap_edited);

        p.setRenderHint(QPainter::SmoothPixmapTransform);
        p.translate(transformedRect.width() / 2, transformedRect.height() / 2);
        p.rotate(current);
        p.translate(-transformedRect.width() / 2, -transformedRect.height() / 2);
        p.drawPixmap((transformedRect.width() - m_pixmap.width()) / 2, (transformedRect.height() - m_pixmap.height()) / 2, m_pixmap);
        p.end();
    }

    const QPointF p = m_data->pointToOrigin((m_img_size.width() * m_data->getZoom()), (m_img_size.height() * m_data->getZoom()), this->size());
    m_data->m_boundingRect = QRectF(p.x(), p.y(), (m_img_size.width() * m_data->getZoom()), (m_img_size.height() * m_data->getZoom()));

    m_data->avoidOutOfScreen(this->size());
    m_data->updateSize(this->size());
    this->update();
}

void PictureItemRaster::setZoom(const qreal current, const qreal /*previous*/)
{
    const QPointF p = m_data->pointToOrigin((m_img_size.width() * current), (m_img_size.height() * current), this->size());

    m_data->m_boundingRect = QRectF(p.x(), p.y(), (m_img_size.width() * current), (m_img_size.height() * current));

    m_data->avoidOutOfScreen(this->size());
    m_data->updateSize(this->size());
    this->update();
}
