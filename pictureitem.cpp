#include "pictureitem.h"

#include <QtGui/qboxlayout.h>

PictureItem::PictureItem(bool opengl, QWidget * parent, Qt::WindowFlags f)
{
    QVBoxLayout *vboxMain = new QVBoxLayout(this);
    vboxMain->setSpacing(0);
    vboxMain->setMargin(0);

    this->opengl = opengl;
    if(this->opengl == true)
    {
        imageDisplayGL = new PictureItemGL();
        connect(imageDisplayGL, SIGNAL(toggleFullscreen()), this, SIGNAL(toggleFullscreen()));
        connect(imageDisplayGL, SIGNAL(zoomChanged()), this, SIGNAL(zoomChanged()));
        connect(imageDisplayGL, SIGNAL(pageNext()), this, SIGNAL(pageNext()));
        connect(imageDisplayGL, SIGNAL(pagePrevious()), this, SIGNAL(pagePrevious()));

        imageDisplayRaster = NULL;
        vboxMain->addWidget(imageDisplayGL);
    }
    else
    {
        imageDisplayRaster = new PictureItemRaster();
        connect(imageDisplayRaster, SIGNAL(toggleFullscreen()), this, SIGNAL(toggleFullscreen()));
        connect(imageDisplayRaster, SIGNAL(zoomChanged()), this, SIGNAL(zoomChanged()));
        connect(imageDisplayRaster, SIGNAL(pageNext()), this, SIGNAL(pageNext()));
        connect(imageDisplayRaster, SIGNAL(pagePrevious()), this, SIGNAL(pagePrevious()));

        imageDisplayGL = NULL;
        vboxMain->addWidget(imageDisplayRaster);
    }
    this->setLayout(vboxMain);
}


QPixmap PictureItem::getPixmap()
{
    if(opengl == true)
    {
        return imageDisplayGL->getPixmap();
    }
    else
    {
        return imageDisplayRaster->getPixmap();
    }
}

void PictureItem::setZoom(qreal z)
{
    if(opengl == true)
    {
        imageDisplayGL->setZoom(z);
    }
    else
    {
        imageDisplayRaster->setZoom(z);
    }
}

qreal PictureItem::getZoom()
{
    if(opengl == true)
    {
        return imageDisplayGL->getZoom();
    }
    else
    {
        return imageDisplayRaster->getZoom();
    }
}

void PictureItem::setPixmap(const QPixmap &p)
{
    if(opengl == true)
    {
        imageDisplayGL->setPixmap(p);
    }
    else
    {
        imageDisplayRaster->setPixmap(p);
    }
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
    if(opengl == true)
    {
        return imageDisplayGL->getRotation();
    }
    else
    {
        return imageDisplayRaster->getRotation();
    }
}

void PictureItem::setLockMode(LockMode::Mode mode)
{
    if(opengl == true)
    {
        imageDisplayGL->setLockMode(mode);
    }
    else
    {
        imageDisplayRaster->setLockMode(mode);
    }
}

QVector<qreal> PictureItem::getDefaultZoomSizes()
{
    if(opengl == true)
    {
        return imageDisplayGL->getDefaultZoomSizes();
    }
    else
    {
        return imageDisplayRaster->getDefaultZoomSizes();
    }
}

LockMode::Mode PictureItem::getLockMode()
{
    if(opengl == true)
    {
        return imageDisplayGL->getLockMode();
    }
    else
    {
        return imageDisplayRaster->getLockMode();
    }
}

void PictureItem::zoomIn()
{
    if(opengl == true)
    {
        imageDisplayGL->zoomIn();
    }
    else
    {
        imageDisplayRaster->zoomIn();
    }
}

void PictureItem::zoomOut()
{
    if(opengl == true)
    {
        imageDisplayGL->zoomOut();
    }
    else
    {
        imageDisplayRaster->zoomOut();
    }
}

void PictureItem::fitToScreen()
{
    if(opengl == true)
    {
        imageDisplayGL->fitToScreen();
    }
    else
    {
        imageDisplayRaster->fitToScreen();
    }
}

void PictureItem::fitWidth()
{
    if(opengl == true)
    {
        imageDisplayGL->fitWidth();
    }
    else
    {
        imageDisplayRaster->fitWidth();
    }
}

void PictureItem::fitHeight()
{
    if(opengl == true)
    {
        imageDisplayGL->fitHeight();
    }
    else
    {
        imageDisplayRaster->fitHeight();
    }
}
