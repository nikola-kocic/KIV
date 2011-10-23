#include "pictureitem_raster.h"
#include "settings.h"
#include "picture_loader.h"

//#include <QtCore/qdebug.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

PictureItemRaster::PictureItemRaster(PictureItemShared *picItemShared, QWidget *parent, Qt::WindowFlags f)
{
    this->picItemShared = picItemShared;
    connect(this->picItemShared, SIGNAL(zoomChanged(qreal,qreal)), this, SLOT(setZoom(qreal,qreal)));

    imageLoad = new QFutureWatcher<QPixmap>(this);
    connect(imageLoad, SIGNAL(resultReadyAt(int)), this, SLOT(imageFinished(int)));
}

void PictureItemRaster::imageFinished(int num)
{
    this->pixmap = imageLoad->resultAt(num);
    picItemShared->setPixmapNull(this->pixmap.isNull());

    this->pixmap_edited = this->pixmap;

    this->picItemShared->boundingRect = QRect(0, 0, this->pixmap.width(), this->pixmap.height());
    if (this->picItemShared->getLockMode() != LockMode::Zoom)
    {
        this->picItemShared->setZoom(1);
    }
    this->picItemShared->setRotation(0);

    this->picItemShared->afterPixmapLoad();

    this->update();

    emit imageChanged();
}

void PictureItemRaster::setFile(const FileInfo &info)
{
    imageLoad->setFuture(QtConcurrent::run(PictureLoader::getPixmap, info));
}

void PictureItemRaster::paintEvent(QPaintEvent *event)
{
    if (this->picItemShared->isPixmapNull())
    {
        return;
    }

    QPainter p(this);
    p.setClipRect(event->region().boundingRect());
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    qreal zoom = this->picItemShared->getZoom();
    QRectF sourceRect = QRectF(-this->picItemShared->boundingRect.x() / zoom,
                               -this->picItemShared->boundingRect.y() / zoom,
                               event->region().boundingRect().width() / zoom,
                               event->region().boundingRect().height() / zoom);

    QRectF drawRect = (QRectF)event->region().boundingRect();

    if (drawRect.width() > (this->pixmap_edited.width() * this->picItemShared->getZoom()))
    {
        drawRect.moveLeft((drawRect.width() - (this->pixmap_edited.width() * zoom)) / 2);
    }
    if (drawRect.height() > (this->pixmap_edited.height() * this->picItemShared->getZoom()))
    {
        drawRect.moveTop((drawRect.height() - (this->pixmap_edited.height() * zoom)) / 2);
    }

    p.drawPixmap(drawRect, this->pixmap_edited, sourceRect);
    p.end();
}


//Region Rotation

void PictureItemRaster::setRotation(qreal r)
{
    if (this->picItemShared->isPixmapNull())
    {
        return;
    }

    this->picItemShared->setRotation(r);

    if ((int)this->picItemShared->getRotation() % 360 == 0)
    {
        this->pixmap_edited = this->pixmap;
    }
    else
    {
        QTransform tRot;
        tRot.rotate(this->picItemShared->getRotation());

        Qt::TransformationMode rotateMode;
        rotateMode = Qt::SmoothTransformation;
    //    rotateMode = Qt::FastTransformation;

        this->pixmap_edited = this->pixmap.transformed(tRot, rotateMode);
    }

    this->picItemShared->boundingRect.setWidth(this->pixmap_edited.width() * this->picItemShared->getZoom());
    this->picItemShared->boundingRect.setHeight(this->pixmap_edited.height() * this->picItemShared->getZoom());
    this->picItemShared->avoidOutOfScreen();
    this->update();
}


//End Region Rotation



//Region Zoom

void PictureItemRaster::setZoom(qreal current, qreal previous)
{
    if (this->picItemShared->isPixmapNull())
    {
        return;
    }

    QPointF p = this->picItemShared->pointToOrigin((this->pixmap_edited.width() * current), (this->pixmap_edited.height() * current));

    this->picItemShared->boundingRect = QRectF(p.x(), p.y(), (this->pixmap_edited.width() * current), (this->pixmap_edited.height() * current));

    this->picItemShared->avoidOutOfScreen();
    this->update();

    emit this->zoomChanged();
}

//End Region Zoom



//Region Drag


