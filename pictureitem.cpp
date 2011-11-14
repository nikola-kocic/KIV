#include "pictureitem.h"
#include "settings.h"

#include <QtGui/qevent.h>


PictureItem::PictureItem(bool opengl, QWidget *parent, Qt::WindowFlags f)
{
    this->opengl = opengl;
    setCursor(Qt::OpenHandCursor);

    QVBoxLayout *vboxMain = new QVBoxLayout(this);
    vboxMain->setSpacing(0);
    vboxMain->setMargin(0);
    this->imageDisplayRaster = 0;
    this->imageDisplayGL = 0;

    this->timerScrollPage = new QTimer(this);
    connect(this->timerScrollPage, SIGNAL(timeout()), this, SLOT(on_timerScrollPage_timeout()));

    this->defaultZoomSizes << 0.1 << 0.25 << 0.5 <<  0.75 << 1 << 1.25 << 1.5 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10;
    this->flagJumpToEnd = false;
    this->zoom = 1;
    this->dragging = false;
    this->rotation = 0;
    this->lockMode = LockMode::None;

    this->textureLoader = new QFutureWatcher<QImage>(this);
    connect(this->textureLoader, SIGNAL(resultReadyAt(int)), this, SLOT(textureFinished(int)));

    this->imageLoader = new QFutureWatcher<QImage>(this);
    connect(this->imageLoader, SIGNAL(resultReadyAt(int)), this, SLOT(imageFinished(int)));


    initPictureItem();

    this->setLayout(vboxMain);
}


void PictureItem::initPictureItem()
{
    if (this->opengl)
    {
        this->imageDisplayGL = new PictureItemGL(this);
        this->imageDisplayRaster = 0;
        this->layout()->addWidget(this->imageDisplayGL);
    }
    else
    {
        this->imageDisplayRaster = new PictureItemRaster(this);
        this->imageDisplayGL = 0;
        this->layout()->addWidget(this->imageDisplayRaster);
    }
}

bool PictureItem::getHardwareAcceleration()
{
    return this->opengl;
}

void PictureItem::setHardwareAcceleration(bool b)
{
    if (this->opengl != b)
    {
        if (this->opengl)
        {
            this->imageDisplayGL->disconnect();
            this->imageDisplayGL->deleteLater();
        }
        else
        {
            this->imageDisplayRaster->disconnect();
            this->imageDisplayRaster->deleteLater();
        }

        this->opengl = b;
        initPictureItem();
    }
}


bool PictureItem::isPixmapNull()
{
    return this->pixmapNull;
}

void PictureItem::setPixmapNull(bool value)
{
    this->pixmapNull = value;
}


qreal PictureItem::getRotation()
{
    return this->rotation;
}

void PictureItem::setRotation(qreal r)
{
    if (isPixmapNull())
    {
        return;
    }

    this->rotation = r;

    if (this->opengl)
    {
        this->imageDisplayGL->setRotation(r);
    }
    else
    {
        this->imageDisplayRaster->setRotation(r);
    }
}


qreal PictureItem::getZoom()
{
    return this->zoom;
}

void PictureItem::setZoom(qreal z)
{
    if (isPixmapNull())
    {
        return;
    }

    if (z < 0.001)
    {
        z = 0.001;
    }
    else if (z > 1000)
    {
        z = 1000;
    }

    qreal previous = this->zoom;
    this->zoom = z;

    if (this->opengl)
    {
        this->imageDisplayGL->setZoom(this->zoom, previous);
    }
    else
    {
        this->imageDisplayRaster->setZoom(this->zoom, previous);
    }
}


LockMode::Mode PictureItem::getLockMode()
{
    return this->lockMode;
}

void PictureItem::setLockMode(LockMode::Mode mode)
{
    this->lockMode = mode;
    this->updateLockMode();
}


QVector<qreal> PictureItem::getDefaultZoomSizes()
{
    return this->defaultZoomSizes;
}









void PictureItem::setPixmap(const FileInfo &info)
{
    this->returnTexCount = 0;
    if (info.imageFileName.isEmpty() && info.zipImageFileName.isEmpty())
    {
        if (opengl)
        {
            this->imageDisplayGL->setImage(QImage());
        }
        else
        {
            this->imageDisplayRaster->setImage(QImage());
        }
        emit imageChanged();
    }
    else
    {
#ifdef DEBUG_PICTUREITEM
    t.start();
#endif
        this->imageLoader->setFuture(QtConcurrent::run(PictureLoader::getImage, info));
    }
}

void PictureItem::imageFinished(int num)
{
#ifdef DEBUG_PICTUREITEM
    qDebug() << "loaded image" << t.restart();
#endif
    this->setPixmapNull(this->imageLoader->resultAt(num).isNull());
    if (this->opengl)
    {
        this->imageDisplayGL->setImage(this->imageLoader->resultAt(num));
    }
    else
    {
        this->imageDisplayRaster->setImage(this->imageLoader->resultAt(num));
        emit imageChanged();
    }

    //Free result memory
    this->imageLoader->setFuture(QFuture<QImage>());
}

void PictureItem::loadTextures(QList<TexIndex> indexes)
{
    returnTexCount = indexes.size();
    this->textureLoader->setFuture(QtConcurrent::mapped(indexes, TexImg::CreatePow2Bitmap));
}

void PictureItem::textureFinished(int num)
{
    if (this->opengl)
    {
        this->imageDisplayGL->setTexture(this->textureLoader->resultAt(num), num);
        if (--this->returnTexCount == 0)
        {
            this->setPixmapNull(false);
            this->textureLoader->setFuture(QFuture<QImage>());
            this->imageDisplayGL->textureLoadFinished();

#ifdef DEBUG_PICTUREITEM
            qDebug() << "loaded textures" << t.elapsed();
#endif
            emit imageChanged();
        }
    }
}

void PictureItem::afterPixmapLoad()
{
    this->updateLockMode();

    if (this->boundingRect.width() > this->size().width())
    {
        if ((Settings::Instance()->getRightToLeft() && !this->flagJumpToEnd)
                || (!Settings::Instance()->getRightToLeft() && this->flagJumpToEnd)

                )
        {
            this->boundingRect.moveLeft(-(this->boundingRect.width() - this->size().width()));
        }
    }

    if (this->flagJumpToEnd)
    {
        if (this->boundingRect.height() > this->size().height())
        {
            this->boundingRect.moveTop(-(this->boundingRect.height() - this->size().height()));
        }
        this->flagJumpToEnd = false;
    }
}





void PictureItem::mousePressEvent(QMouseEvent *ev)
{
    this->setFocus();

    if (ev->button() ==  Qt::RightButton)
    {
        if (ev->buttons() == (Qt::LeftButton | Qt::RightButton))
        {
            emit pageNext();
//            _HandledPageChange
        }
    }
    else if (ev->button() == Qt::LeftButton
//             || LockDrag
             )
    {
        if (ev->buttons() == (Qt::LeftButton | Qt::RightButton))
        {
            emit pagePrevious();
//            _HandledPageChange = true;
        }
        else
        {

            // Start dragging
            this->beginDrag(ev->pos());
        }
    }
    else if (ev->button() == Qt::MiddleButton)
    {
        switch(Settings::Instance()->getMiddleClick())
        {
        case MiddleClick::Fullscreen :
            emit(toggleFullscreen());
            break;

        case MiddleClick::AutoFit:
            this->fitToScreen();
            break;

        case MiddleClick::ZoomReset:
            this->setZoom(1);
            break;

        case MiddleClick::NextPage:
            emit pageNext();
            break;

//        case MiddleClick::FollowMouse:

//            LockDrag = !LockDrag;
//            break;

        default: break;

        }
    }
}

void PictureItem::mouseMoveEvent(QMouseEvent *ev)
{
    this->drag(ev->pos());
}

void PictureItem::mouseReleaseEvent(QMouseEvent *ev)
{
    if (this->dragging && ev->button() == Qt::LeftButton)
    {
        this->endDrag();
        this->setCursor(Qt::OpenHandCursor);
    }
}

//End Region Drag



void PictureItem::resizeEvent(QResizeEvent *)
{
    if (this->isPixmapNull())
    {
        return;
    }

    this->avoidOutOfScreen();
    this->updateLockMode();
}


void PictureItem::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Up)
    {
        this->ScrollPageVertical(120);
        ev->accept();
    }
    else if (ev->key() == Qt::Key_Down)
    {
        this->ScrollPageVertical(-120);
        ev->accept();
    }
    else if (ev->key() == Qt::Key_Left)
    {
        this->ScrollPageHorizontal(120);
        ev->accept();
    }
    else if (ev->key() == Qt::Key_Right)
    {
        this->ScrollPageHorizontal(-120);
        ev->accept();
    }
}

void PictureItem::wheelEvent(QWheelEvent *event)
{    if (event->modifiers() == Qt::ControlModifier
         || (event->modifiers() == Qt::NoModifier && Settings::Instance()->getWheel() == Wheel::Zoom)
         )
    {
        if (event->delta() < 0)
        {
            this->zoomOut();
        }
        else
        {
            this->zoomIn();
        }
    }
    else if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
    {
        this->setZoom(this->zoom * (1 + ((event->delta() / 4.8) / 100)));
    }
    else if (event->modifiers() == Qt::ShiftModifier)
    {
        this->ScrollPageVertical(event->delta());
    }
    else if (event->modifiers() == Qt::AltModifier)
    {
        this->ScrollPageHorizontal(event->delta());
    }
    else if (event->modifiers() == Qt::NoModifier)
    {
        if (Settings::Instance()->getWheel() == Wheel::ChangePage)
        {
            if (event->delta() < 0)
            {
                emit pageNext();
            }
            else
            {
                emit pagePrevious();
            }
        }
        else if (Settings::Instance()->getWheel() == Wheel::Scroll)
        {
            if (this->boundingRect.height() > this->size().height() || this->boundingRect.width() > this->size().width())
            {
                //If we scroll to bottom of page, start timer
                if (event->delta() < 0 && -this->boundingRect.y() + this->size().height() >= this->boundingRect.height() && !this->timerScrollPage->isActive())
                {
                    if (Settings::Instance()->getScrollPageByWidth())
                    {
                        if (Settings::Instance()->getRightToLeft())
                        {
                            if (this->boundingRect.x() < 0)
                            {
                                this->ScrollPageHorizontal(-event->delta());

                                if (this->boundingRect.x() == 0)
                                {
                                    this->start_timerScrollPage();
                                }
                                return;
                            }
                        }
                        else
                        {
                            if ((this->boundingRect.width() + this->boundingRect.x()) > this->size().width())
                            {
                                this->ScrollPageHorizontal(event->delta());


                                if (this->boundingRect.width() + this->boundingRect.x() == this->size().width())
                                {
                                    this->start_timerScrollPage();
                                }
                                return;
                            }
                        }
                    }
                    if ((Settings::Instance()->getScrollChangesPage()) && !this->timerScrollPage->isActive())
                    {
                        this->start_timerScrollPage();
                        emit pageNext();
                    }
                }
                else if ((Settings::Instance()->getScrollChangesPage()) && (event->delta() > 0 && this->boundingRect.y() == 0 && !this->timerScrollPage->isActive()))
                {
                    this->start_timerScrollPage();
                    this->flagJumpToEnd = Settings::Instance()->getJumpToEnd();
                    emit pagePrevious();
                }
                else
                {

                    // Keep dragging
                    ScrollPageVertical(event->delta());

                    if ((this->boundingRect.height() + this->boundingRect.y() == this->size().height()
                         || this->boundingRect.y() == 0)
                            && !this->timerScrollPage->isActive()
                            )
                    {
                        this->start_timerScrollPage();
                    }
                }
            }
            else
            {
                if (Settings::Instance()->getScrollChangesPage())
                {
                    if (event->delta() > 0)
                    {
                        emit pagePrevious();
                    }
                    else if (event->delta() < 0)
                    {
                        emit pageNext();
                    }
                }
            }
        }
    }
    this->update();
}


void PictureItem::start_timerScrollPage()
{
    if (Settings::Instance()->getPageChangeTimeout() > 0)
    {
        this->timerScrollPage->start(Settings::Instance()->getPageChangeTimeout());
    }
}

void PictureItem::on_timerScrollPage_timeout()
{
    this->timerScrollPage->stop();
}

//End Region Rotation



//Region Zoom



QPoint PictureItem::pointToOrigin(int width, int height)
{
    qreal zoomX = (qreal)width / (qreal)this->boundingRect.width();
    qreal zoomY = (qreal)height / (qreal)this->boundingRect.height();

    if (width > this->size().width())
    {
        qreal oldX = (this->boundingRect.x() - (this->boundingRect.x() * 2)) + (this->size().width() / 2);
        qreal oldY = (this->boundingRect.y() - (this->boundingRect.y() * 2)) + (this->size().height() / 2);

        qreal newX = oldX * zoomX;
        qreal newY = oldY * zoomY;

        qreal originX = newX - (this->size().width() / 2) - ((newX - (this->size().width() / 2)) * 2);
        qreal originY = newY - (this->size().height() / 2) - ((newY - (this->size().height() / 2)) * 2);

        return QPoint(originX, originY);
    }
    else
    {
        if (height > this->size().height())
        {
            qreal oldY = (this->boundingRect.y() - (this->boundingRect.y() * 2)) + (this->size().height() / 2);

            qreal newY = oldY * zoomY;

            qreal originY = newY - (this->size().height() / 2) - ((newY - (this->size().height() / 2)) * 2);

            return QPoint(0, originY);
        }
        else
        {
            return QPoint(0, 0);
        }
    }
}


void PictureItem::zoomIn()
{
    for (int i = 0; i < this->defaultZoomSizes.count(); ++i)
    {
        if (this->defaultZoomSizes.at(i) > this->zoom)
        {
            this->setZoom(this->defaultZoomSizes.at(i));
            return;
        }
    }

    this->setZoom(this->zoom * 1.25);
}

void PictureItem::zoomOut()
{
    for (int i = 0; i < this->defaultZoomSizes.count(); ++i)
    {
        if (this->defaultZoomSizes.at(i) >= this->zoom)
        {
            if (i != 0)
            {
                this->setZoom(this->defaultZoomSizes.at(i - 1));
            }
            else
            {
                this->setZoom(this->zoom / 1.25);
            }
            return;
        }
    }

    setZoom(this->zoom / 1.25);
}

void PictureItem::fitToScreen()
{
    if (this->isPixmapNull())
    {
        return;
    }

    QRect temp = QRect(this->boundingRect.x(), this->boundingRect.y(), this->boundingRect.width() / this->zoom, this->boundingRect.height() / this->zoom);

    qreal x_ratio = (qreal)this->size().width() / temp.width();
    qreal y_ratio = (qreal)this->size().height() / temp.height();

    if ((temp.width() <= this->size().width()) && (temp.height() <= this->size().height()))
    {
        this->setZoom(1);
    }
    else if ((x_ratio * temp.height()) < this->size().height())
    {
        this->setZoom(x_ratio);
    }
    else
    {
        this->setZoom(y_ratio);
    }
}

void PictureItem::fitWidth()
{
    if (this->isPixmapNull())
    {
        return;
    }

    qreal tw = this->boundingRect.width() / this->zoom;

    qreal x_ratio = (qreal)this->size().width() / tw;

    if (tw <= this->size().width())
    {
        this->setZoom(1);
    }
    else
    {
        this->setZoom(x_ratio);
    }
}

void PictureItem::fitHeight()
{
    if (this->isPixmapNull())
    {
        return;
    }

    qreal th = this->boundingRect.height() / this->zoom;

    qreal y_ratio = (qreal)this->size().height() / th;

    if (th <= this->size().height())
    {
        this->setZoom(1);
    }
    else
    {
        this->setZoom(y_ratio);
    }

}

void PictureItem::updateLockMode()
{
    if (this->isPixmapNull())
    {
        return;
    }

    //use in setpixmap and resize events
    switch (this->lockMode)
    {
    case LockMode::Autofit:
        this->fitToScreen();
        break;
    case LockMode::FitWidth:
        this->fitWidth();
        break;
    case LockMode::FitHeight:
        this->fitHeight();
        break;
    default:
        break;
    }
}


void PictureItem::avoidOutOfScreen()
{
    if (this->isPixmapNull())
    {
        return;
    }

    // Am I lined out to the left?
    if (this->boundingRect.x() >= 0)
    {
        this->boundingRect.moveLeft(0);
    }
    else if ((this->boundingRect.x() <= (this->boundingRect.width() - this->size().width()) - ((this->boundingRect.width() - this->size().width()) * 2)))
    {
        if ((this->boundingRect.width() - this->size().width()) - ((this->boundingRect.width() - this->size().width()) * 2) <= 0)
        {
            // I am too far to the left!
            this->boundingRect.moveLeft((this->boundingRect.width() - this->size().width()) - ((this->boundingRect.width() - this->size().width()) * 2));
        }
        else
        {
            // I am too far to the right!
            this->boundingRect.moveLeft(0);
        }
    }

    // Am I lined out to the top?
    if (this->boundingRect.y() >= 0)
    {
        this->boundingRect.moveTop(0);
    }
    else if ((this->boundingRect.y() <= (this->boundingRect.height() - this->size().height()) - ((this->boundingRect.height() - this->size().height()) * 2)))
    {
        if ((this->boundingRect.height() - this->size().height()) - ((this->boundingRect.height() - this->size().height()) * 2) <= 0)
        {
            // I am too far to the top!
            this->boundingRect.moveTop((this->boundingRect.height() - this->size().height()) - ((this->boundingRect.height() - this->size().height()) * 2));
        }
        else
        {
            // I am too far to the bottom!
            this->boundingRect.moveTop(0);
        }
    }
}

//End Region Zoom



//Region Drag

void PictureItem::drag(const QPoint &pt)
{
    if (this->isPixmapNull())
    {
        return;
    }

    if (this->dragging)
    {
        // Am I dragging it outside of the panel?
        if ((pt.x() - this->dragPoint.x() >= (this->boundingRect.width() - this->size().width()) - ((this->boundingRect.width() - this->size().width()) * 2)) && (pt.x() - this->dragPoint.x() <= 0))
        {
            // No, everything is just fine
            this->boundingRect.moveLeft(pt.x() - this->dragPoint.x());
        }
        else if ((pt.x() - this->dragPoint.x() > 0))
        {
            // Now don't drag it out of the panel please
            this->boundingRect.moveLeft(0);
        }
        else if ((pt.x() - this->dragPoint.x() < (this->boundingRect.width() - this->size().width()) - ((this->boundingRect.width() - this->size().width()) * 2)))
        {
            // I am dragging it out of my panel. How many pixels do I have left?
            if ((this->boundingRect.width() - this->size().width()) - ((this->boundingRect.width() - this->size().width()) * 2) <= 0)
            {
                // Make it fit perfectly
                this->boundingRect.moveLeft((this->boundingRect.width() - this->size().width()) - ((this->boundingRect.width() - this->size().width()) * 2));
            }
        }

        // Am I dragging it outside of the panel?
        if (pt.y() - this->dragPoint.y() >= (this->boundingRect.height() - this->size().height()) - ((this->boundingRect.height() - this->size().height()) * 2) && (pt.y() - this->dragPoint.y() <= 0))
        {
            // No, everything is just fine
            this->boundingRect.moveTop(pt.y() - this->dragPoint.y());
        }
        else if ((pt.y() - this->dragPoint.y() > 0))
        {
            // Now don't drag it out of the panel please
            this->boundingRect.moveTop(0);
        }
        else if (pt.y() - this->dragPoint.y() < (this->boundingRect.height() - this->size().height()) - ((this->boundingRect.height() - this->size().height()) * 2))
        {
            // I am dragging it out of my panel. How many pixels do I have left?
            if ((this->boundingRect.height() - this->size().height()) - ((this->boundingRect.height() - this->size().height()) * 2) <= 0)
            {
                // Make it fit perfectly
                this->boundingRect.moveTop((this->boundingRect.height() - this->size().height()) - ((this->boundingRect.height()- this->size().height()) * 2));
            }
        }

        if (this->opengl)
        {
            this->imageDisplayGL->update();
        }
        else
        {
            this->imageDisplayRaster->update();
        }
    }
}

void PictureItem::beginDrag(const QPoint &pt)
{
    if (this->isPixmapNull())
    {
        return;
    }

    // Initial drag position
    this->dragPoint.setX(pt.x() - this->boundingRect.x());
    this->dragPoint.setY(pt.y() - this->boundingRect.y());
    this->dragging = true;
    this->setCursor(Qt::ClosedHandCursor);
}

void PictureItem::endDrag()
{
    if (this->isPixmapNull())
    {
        return;
    }

    this->dragging = false;
    this->setCursor(Qt::OpenHandCursor);
}

//End Region Drag

void PictureItem::ScrollPageVertical(int value)
{
    this->beginDrag(QPoint(0,0));
    this->drag(QPoint(0,value));
    this->endDrag();
}

void PictureItem::ScrollPageHorizontal(int value)
{
    this->beginDrag(QPoint(0,0));
    this->drag(QPoint(value,0));
    this->endDrag();
}
