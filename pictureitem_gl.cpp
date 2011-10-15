#include "pictureitem_gl.h"
#include "settings.h"

//#include <QtCore/qdebug.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtGui/qevent.h>

PictureItemGL::PictureItemGL( PictureItemShared* pis, QWidget * parent, Qt::WindowFlags f )
{
    this->pis = pis;
    connect(pis, SIGNAL(pixmapChanged()), this, SLOT(setPixmap()));
    connect(pis, SIGNAL(zoomChanged(qreal,qreal)), this, SLOT(setZoom(qreal,qreal)));
    connect(pis, SIGNAL(update()), this, SLOT(update()));
}

void PictureItemGL::setPixmap()
{
    setRotation(0);
    if(pis->getLockMode() != LockMode::Zoom)
    {
        pis->setZoom(1);
    }
    pis->boundingRect = QRect(0, 0, pis->getPixmapSize().width() * pis->getZoom(), pis->getPixmapSize().height() * pis->getZoom());

    pis->afterPixmapLoad();

    update();
}

void PictureItemGL::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setClipRect(event->region().boundingRect());
    QPalette obj;
    QBrush bg = obj.window();
    p.fillRect(event->region().boundingRect(), bg);

    if(! pis->isPixmapNull() )
    {
        qreal x = event->region().boundingRect().x();
        qreal y = event->region().boundingRect().y();
        qreal w = event->region().boundingRect().width();
        qreal h = event->region().boundingRect().height();

        if(w  > pis->boundingRect.width())
        {
            x = (w - pis->boundingRect.width()) / 2;
        }
        if(h  > pis->boundingRect.height())
        {
            y = (h - pis->boundingRect.height()) / 2;
        }

        p.setRenderHint(QPainter::SmoothPixmapTransform);

        p.translate(pis->boundingRect.x() + translatePoint.x() + x, pis->boundingRect.y() + translatePoint.y() + y);
        p.scale(pis->getZoom(),pis->getZoom());
        p.translate(pis->getPixmapSize().height()/2, pis->getPixmapSize().width()/2);
        p.rotate(pis->getRotation());
        p.translate(-pis->getPixmapSize().height()/2, -pis->getPixmapSize().width()/2);

        p.drawPixmap(0, 0, pis->getPixmap());
    }

    p.end();
}


//Region Rotation

void PictureItemGL::setRotation(qreal r)
{
    if( pis->isPixmapNull() ) return;

    pis->rotation = r;

    QTransform tRot;
    tRot.translate(pis->boundingRect.x(), pis->boundingRect.y());
    tRot.scale(pis->getZoom(),pis->getZoom());
    tRot.translate(pis->getPixmapSize().height()/2, pis->getPixmapSize().width()/2);
    tRot.rotate(pis->getRotation());
    tRot.translate(-pis->getPixmapSize().height()/2, -pis->getPixmapSize().width()/2);
    QRect transformedRot = tRot.mapRect(pis->getPixmap().rect());

    translatePoint = QPoint(pis->boundingRect.x() - transformedRot.x(), pis->boundingRect.y() - transformedRot.y());
    pis->boundingRect.setWidth(transformedRot.width());
    pis->boundingRect.setHeight(transformedRot.height());

    if(pis->boundingRect.height() + pis->boundingRect.y() < this->height())
    {
        pis->boundingRect.translate(0, this->height() - (pis->boundingRect.height() + pis->boundingRect.y()));
    }

    if(pis->boundingRect.height()  < this->height())
    {
        pis->boundingRect.moveTop(0);
    }

    update();
}


//End Region Rotation



//Region Zoom

void PictureItemGL::setZoom(qreal current, qreal previous)
{
    if( pis->isPixmapNull() ) return;

    QPointF p = pis->pointToOrigin( (pis->boundingRect.width() / previous) * current, (pis->boundingRect.height() / previous) * current );
    pis->boundingRect = QRectF(p.x(), p.y(), (pis->boundingRect.width() / previous) * current, (pis->boundingRect.height() / previous) * current);

    setRotation(pis->getRotation());
    pis->avoidOutOfScreen();

    update();

    emit zoomChanged();
}

