#include "pictureitem.h"
#include "settings.h"
#include "picture_loader.h"

//#include <QtCore/qdebug.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

PictureItem::PictureItemRaster::PictureItemRaster(PictureItem *parent, Qt::WindowFlags f)
{
    this->picItem = parent;
}

void PictureItem::PictureItemRaster::setImage(QImage img)
{
    this->setUpdatesEnabled(false);
    this->pixmap = QPixmap::fromImage(img);

    this->pixmap_edited = this->pixmap;

    this->picItem->boundingRect = QRect(0, 0, this->pixmap.width(), this->pixmap.height());
    if (this->picItem->getLockMode() != LockMode::Zoom)
    {
        this->picItem->setZoom(1);
    }
    this->picItem->setRotation(0);

    this->picItem->afterPixmapLoad();

    this->setUpdatesEnabled(true);
    this->update();
}

void PictureItem::PictureItemRaster::paintEvent(QPaintEvent *event)
{
    if (this->picItem->isPixmapNull())
    {
        return;
    }

    QPainter p(this);
    p.setClipRect(event->region().boundingRect());
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    qreal zoom = this->picItem->getZoom();
    QRectF sourceRect = QRectF(-this->picItem->boundingRect.x() / zoom,
                               -this->picItem->boundingRect.y() / zoom,
                               event->region().boundingRect().width() / zoom,
                               event->region().boundingRect().height() / zoom);

    QRectF drawRect = (QRectF)event->region().boundingRect();

    if (drawRect.width() > (this->pixmap_edited.width() * this->picItem->getZoom()))
    {
        drawRect.moveLeft((drawRect.width() - (this->pixmap_edited.width() * zoom)) / 2);
    }
    if (drawRect.height() > (this->pixmap_edited.height() * this->picItem->getZoom()))
    {
        drawRect.moveTop((drawRect.height() - (this->pixmap_edited.height() * zoom)) / 2);
    }

    p.drawPixmap(drawRect, this->pixmap_edited, sourceRect);
    p.end();
}

void PictureItem::PictureItemRaster::setRotation(qreal r)
{
    if ((int)this->picItem->getRotation() % 360 == 0)
    {
        this->pixmap_edited = this->pixmap;
    }
    else
    {
        QTransform tRot;
        tRot.rotate(this->picItem->getRotation());

        Qt::TransformationMode rotateMode;
        rotateMode = Qt::SmoothTransformation;
    //    rotateMode = Qt::FastTransformation;

        this->pixmap_edited = this->pixmap.transformed(tRot, rotateMode);
    }

    this->picItem->boundingRect.setWidth(this->pixmap_edited.width() * this->picItem->getZoom());
    this->picItem->boundingRect.setHeight(this->pixmap_edited.height() * this->picItem->getZoom());
    this->picItem->avoidOutOfScreen();
    this->update();
}

void PictureItem::PictureItemRaster::setZoom(qreal current, qreal previous)
{
    QPointF p = this->picItem->pointToOrigin((this->pixmap_edited.width() * current), (this->pixmap_edited.height() * current));

    this->picItem->boundingRect = QRectF(p.x(), p.y(), (this->pixmap_edited.width() * current), (this->pixmap_edited.height() * current));

    this->picItem->avoidOutOfScreen();
    this->update();
}
