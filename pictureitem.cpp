#include "pictureitem.h"
#include <QtGui/qevent.h>

PictureItem::PictureItem(bool opengl, QWidget * parent, Qt::WindowFlags f)
{
    this->opengl = opengl;
    setCursor(Qt::OpenHandCursor);
    pis = new PictureItemShared();
    connect(pis, SIGNAL(updateCursor(Qt::CursorShape)), this, SLOT(setMouseCursor(Qt::CursorShape)));
    connect(pis, SIGNAL(update()), this, SLOT(update()));

    vboxMain = new QVBoxLayout(this);
    vboxMain->setSpacing(0);
    vboxMain->setMargin(0);

    initPictureItem();

    this->setLayout(vboxMain);
}

void PictureItem::initPictureItem()
{
    if(this->opengl == true)
    {
        imageDisplayGL = new PictureItemGL(pis);
        connect(pis, SIGNAL(toggleFullscreen()), this, SIGNAL(toggleFullscreen()));
        connect(imageDisplayGL, SIGNAL(zoomChanged()), this, SIGNAL(zoomChanged()));
        connect(pis, SIGNAL(pageNext()), this, SIGNAL(pageNext()));
        connect(pis, SIGNAL(pagePrevious()), this, SIGNAL(pagePrevious()));

        imageDisplayRaster = NULL;
        vboxMain->addWidget(imageDisplayGL);
    }
    else
    {
        imageDisplayRaster = new PictureItemRaster(pis);
        connect(pis, SIGNAL(toggleFullscreen()), this, SIGNAL(toggleFullscreen()));
        connect(imageDisplayRaster, SIGNAL(zoomChanged()), this, SIGNAL(zoomChanged()));
        connect(pis, SIGNAL(pageNext()), this, SIGNAL(pageNext()));
        connect(pis, SIGNAL(pagePrevious()), this, SIGNAL(pagePrevious()));

        imageDisplayGL = NULL;
        vboxMain->addWidget(imageDisplayRaster);
    }
}

void PictureItem::setHardwareAcceleration(bool b)
{
    if(this->opengl != b)
    {
        if(this->opengl == true)
        {

            imageDisplayGL->deleteLater();
        }
        else
        {
            imageDisplayRaster->deleteLater();
        }

        this->opengl = b;
        initPictureItem();
    }
}

bool PictureItem::getHardwareAcceleration()
{
    return opengl;
}

QPixmap PictureItem::getPixmap()
{
    return pis->getPixmap();
}

void PictureItem::setZoom(qreal z)
{
    pis->setZoom(z);
}

qreal PictureItem::getZoom()
{
    return pis->getZoom();
}

void PictureItem::setPixmap(const QPixmap &p)
{
    pis->widgetSize = this->size();
    pis->setPixmap(p);
}

void PictureItem::setRotation(qreal r)
{
    if(opengl == true)
    {
        imageDisplayGL->setRotation(r);
    }
    else
    {
        imageDisplayRaster->setRotation(r);
    }
}

qreal PictureItem::getRotation()
{
    return pis->getRotation();
}

void PictureItem::setLockMode(LockMode::Mode mode)
{
    pis->setLockMode(mode);
}

QVector<qreal> PictureItem::getDefaultZoomSizes()
{
    return pis->getDefaultZoomSizes();
}

LockMode::Mode PictureItem::getLockMode()
{
    return pis->getLockMode();
}

void PictureItem::zoomIn()
{
    pis->zoomIn();
}

void PictureItem::zoomOut()
{
    pis->zoomOut();
}

void PictureItem::fitToScreen()
{
    pis->fitToScreen();
}

void PictureItem::fitWidth()
{
    pis->fitWidth();
}

void PictureItem::fitHeight()
{
    pis->fitHeight();
}



void PictureItem::mousePressEvent(QMouseEvent *ev)
{
    setFocus();

    pis->processMousePressEvent(ev);
}

void PictureItem::mouseMoveEvent(QMouseEvent *ev)
{
    pis->drag( ev->pos() );
}

void PictureItem::mouseReleaseEvent(QMouseEvent *ev)
{
    if(pis->dragging == true && ev->button() == Qt::LeftButton)
    {
        pis->endDrag();
        setCursor(Qt::OpenHandCursor);
    }
}

//End Region Drag



void PictureItem::resizeEvent(QResizeEvent *)
{
    if( pis->isPixmapNull() ) return;

    pis->widgetSize = this->size();
    pis->avoidOutOfScreen();
    pis->updateLockMode();
}


void PictureItem::keyPressEvent(QKeyEvent *ev)
{
    if(pis->processKeyPressEvent(ev->key()) == true)
    {
        ev->accept();
    }
}

void PictureItem::wheelEvent( QWheelEvent *event )
{
    pis->processWheelEvent(event);
    update();
}

void PictureItem::setMouseCursor(Qt::CursorShape cursor)
{
    setCursor(cursor);
}
