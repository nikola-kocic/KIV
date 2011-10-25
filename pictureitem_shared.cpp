#include "pictureitem_raster.h"
#include "settings.h"

//#include <QtCore/qdebug.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

PictureItemShared::PictureItemShared()
{
    this->zoom = 1;
    this->dragging = false;
    this->rotation = 0;
    this->lockMode = LockMode::None;
    this->timerScrollPage = new QTimer();
    connect(timerScrollPage, SIGNAL(timeout()), this, SLOT(on_timerScrollPage_timeout()));
//    LockDrag = false;
    this->flagJumpToEnd = false;
    this->defaultZoomSizes << 0.1 << 0.25 << 0.5 <<  0.75 << 1 << 1.25 << 1.5 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10;
}

//Region Rotation

void PictureItemShared::setRotation(qreal r)
{
    if (isPixmapNull())
    {
        return;
    }

    this->rotation = r;
}


qreal PictureItemShared::getRotation()
{
    return this->rotation;
}

//End Region Rotation



//Region Zoom

void PictureItemShared::setZoom(qreal z)
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

    emit zoomChanged(this->zoom, previous);
}


QPoint PictureItemShared::pointToOrigin(int width, int height)
{
    qreal zoomX = (qreal)width / (qreal)this->boundingRect.width();
    qreal zoomY = (qreal)height / (qreal)this->boundingRect.height();

    if (width > this->widgetSize.width())
    {
        qreal oldX = (this->boundingRect.x() - (this->boundingRect.x() * 2)) + (this->widgetSize.width() / 2);
        qreal oldY = (this->boundingRect.y() - (this->boundingRect.y() * 2)) + (this->widgetSize.height() / 2);

        qreal newX = oldX * zoomX;
        qreal newY = oldY * zoomY;

        qreal originX = newX - (this->widgetSize.width() / 2) - ((newX - (this->widgetSize.width() / 2)) * 2);
        qreal originY = newY - (this->widgetSize.height() / 2) - ((newY - (this->widgetSize.height() / 2)) * 2);

        return QPoint(originX, originY);
    }
    else
    {
        if (height > this->widgetSize.height())
        {
            qreal oldY = (this->boundingRect.y() - (this->boundingRect.y() * 2)) + (this->widgetSize.height() / 2);

            qreal newY = oldY * zoomY;

            qreal originY = newY - (this->widgetSize.height() / 2) - ((newY - (this->widgetSize.height() / 2)) * 2);

            return QPoint(0, originY);
        }
        else
        {
            return QPoint(0, 0);
        }
    }
}

qreal PictureItemShared::getZoom()
{
    return this->zoom;
}

QVector<qreal> PictureItemShared::getDefaultZoomSizes()
{
    return this->defaultZoomSizes;
}

void PictureItemShared::zoomIn()
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

void PictureItemShared::zoomOut()
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

void PictureItemShared::fitToScreen()
{
    if (this->isPixmapNull())
    {
        return;
    }

    QRect temp = QRect(this->boundingRect.x(), this->boundingRect.y(), this->boundingRect.width() / this->zoom, this->boundingRect.height() / this->zoom);

    qreal x_ratio = (qreal)this->widgetSize.width() / temp.width();
    qreal y_ratio = (qreal)this->widgetSize.height() / temp.height();

    if ((temp.width() <= this->widgetSize.width()) && (temp.height() <= this->widgetSize.height()))
    {
        this->setZoom(1);
    }
    else if ((x_ratio * temp.height()) < this->widgetSize.height())
    {
        this->setZoom(x_ratio);
    }
    else
    {
        this->setZoom(y_ratio);
    }
}

void PictureItemShared::fitWidth()
{
    if (this->isPixmapNull())
    {
        return;
    }

    qreal tw = this->boundingRect.width() / this->zoom;

    qreal x_ratio = (qreal)this->widgetSize.width() / tw;

    if (tw <= this->widgetSize.width())
    {
        this->setZoom(1);
    }
    else
    {
        this->setZoom(x_ratio);
    }
}

void PictureItemShared::fitHeight()
{
    if (this->isPixmapNull())
    {
        return;
    }

    qreal th = this->boundingRect.height() / this->zoom;

    qreal y_ratio = (qreal)this->widgetSize.height() / th;

    if (th <= this->widgetSize.height())
    {
        this->setZoom(1);
    }
    else
    {
        this->setZoom(y_ratio);
    }

}

void PictureItemShared::updateLockMode()
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

void PictureItemShared::setLockMode(LockMode::Mode mode)
{
    this->lockMode = mode;
    this->updateLockMode();
}

LockMode::Mode PictureItemShared::getLockMode()
{
    return this->lockMode;
}

void PictureItemShared::avoidOutOfScreen()
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
    else if ((this->boundingRect.x() <= (this->boundingRect.width() - this->widgetSize.width()) - ((this->boundingRect.width() - this->widgetSize.width()) * 2)))
    {
        if ((this->boundingRect.width() - this->widgetSize.width()) - ((this->boundingRect.width() - this->widgetSize.width()) * 2) <= 0)
        {
            // I am too far to the left!
            this->boundingRect.moveLeft((this->boundingRect.width() - this->widgetSize.width()) - ((this->boundingRect.width() - this->widgetSize.width()) * 2));
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
    else if ((this->boundingRect.y() <= (this->boundingRect.height() - this->widgetSize.height()) - ((this->boundingRect.height() - this->widgetSize.height()) * 2)))
    {
        if ((this->boundingRect.height() - this->widgetSize.height()) - ((this->boundingRect.height() - this->widgetSize.height()) * 2) <= 0)
        {
            // I am too far to the top!
            this->boundingRect.moveTop((this->boundingRect.height() - this->widgetSize.height()) - ((this->boundingRect.height() - this->widgetSize.height()) * 2));
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

void PictureItemShared::drag(const QPoint &pt)
{
    if (this->isPixmapNull())
    {
        return;
    }

    if (this->dragging)
    {
        // Am I dragging it outside of the panel?
        if ((pt.x() - this->dragPoint.x() >= (this->boundingRect.width() - this->widgetSize.width()) - ((this->boundingRect.width() - this->widgetSize.width()) * 2)) && (pt.x() - this->dragPoint.x() <= 0))
        {
            // No, everything is just fine
            this->boundingRect.moveLeft(pt.x() - this->dragPoint.x());
        }
        else if ((pt.x() - this->dragPoint.x() > 0))
        {
            // Now don't drag it out of the panel please
            this->boundingRect.moveLeft(0);
        }
        else if ((pt.x() - this->dragPoint.x() < (this->boundingRect.width() - this->widgetSize.width()) - ((this->boundingRect.width() - this->widgetSize.width()) * 2)))
        {
            // I am dragging it out of my panel. How many pixels do I have left?
            if ((this->boundingRect.width() - this->widgetSize.width()) - ((this->boundingRect.width() - this->widgetSize.width()) * 2) <= 0)
            {
                // Make it fit perfectly
                this->boundingRect.moveLeft((this->boundingRect.width() - this->widgetSize.width()) - ((this->boundingRect.width() - this->widgetSize.width()) * 2));
            }
        }

        // Am I dragging it outside of the panel?
        if (pt.y() - this->dragPoint.y() >= (this->boundingRect.height() - this->widgetSize.height()) - ((this->boundingRect.height() - this->widgetSize.height()) * 2) && (pt.y() - this->dragPoint.y() <= 0))
        {
            // No, everything is just fine
            this->boundingRect.moveTop(pt.y() - this->dragPoint.y());
        }
        else if ((pt.y() - this->dragPoint.y() > 0))
        {
            // Now don't drag it out of the panel please
            this->boundingRect.moveTop(0);
        }
        else if (pt.y() - this->dragPoint.y() < (this->boundingRect.height() - this->widgetSize.height()) - ((this->boundingRect.height() - this->widgetSize.height()) * 2))
        {
            // I am dragging it out of my panel. How many pixels do I have left?
            if ((this->boundingRect.height() - this->widgetSize.height()) - ((this->boundingRect.height() - this->widgetSize.height()) * 2) <= 0)
            {
                // Make it fit perfectly
                this->boundingRect.moveTop((this->boundingRect.height() - this->widgetSize.height()) - ((this->boundingRect.height()- this->widgetSize.height()) * 2));
            }
        }
        emit update();
    }
}

void PictureItemShared::processMousePressEvent(QMouseEvent *ev)
{
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

        case MiddleClick::FollowMouse:

            //            LockDrag = !LockDrag;
            break;

        default: break;

        }
    }
}

void PictureItemShared::beginDrag(const QPoint &pt)
{
    if (this->isPixmapNull())
    {
        return;
    }

    // Initial drag position
    this->dragPoint.setX(pt.x() - this->boundingRect.x());
    this->dragPoint.setY(pt.y() - this->boundingRect.y());
    this->dragging = true;
    emit updateCursor(Qt::ClosedHandCursor);
}

void PictureItemShared::endDrag()
{
    if (this->isPixmapNull())
    {
        return;
    }

    this->dragging = false;
    emit updateCursor(Qt::OpenHandCursor);
}

//End Region Drag

void PictureItemShared::ScrollPageVertical(int value)
{
    this->beginDrag(QPoint(0,0));
    this->drag(QPoint(0,value));
    this->endDrag();
}

void PictureItemShared::ScrollPageHorizontal(int value)
{
    this->beginDrag(QPoint(0,0));
    this->drag(QPoint(value,0));
    this->endDrag();
}

bool PictureItemShared::processKeyPressEvent(int key)
{
    if (key == Qt::Key_Up)
    {
        this->ScrollPageVertical(120);
        return true;
    }
    else if (key == Qt::Key_Down)
    {
        this->ScrollPageVertical(-120);
        return true;
    }
    else if (key == Qt::Key_Left)
    {
        this->ScrollPageHorizontal(120);
        return true;
    }
    else if (key == Qt::Key_Right)
    {
        this->ScrollPageHorizontal(-120);
        return true;
    }

    return false;
}

void PictureItemShared::processWheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier
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
            if ((Settings::Instance()->getPageChangeTimeout() > 0) && (this->boundingRect.height() > this->widgetSize.height() || this->boundingRect.width() > this->widgetSize.width()))
            {
                //If we scroll to bottom of page, start timer
                if (event->delta() < 0 && -this->boundingRect.y() + this->widgetSize.height() >= this->boundingRect.height() && !this->timerScrollPage->isActive())
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
                            if ((this->boundingRect.width() + this->boundingRect.x()) > this->widgetSize.width() )
                            {
                                this->ScrollPageHorizontal(event->delta());


                                if (this->boundingRect.width() + this->boundingRect.x() == this->widgetSize.width())
                                {
                                    this->start_timerScrollPage();
                                }
                                return;
                            }
                        }
                    }
                    if (!this->timerScrollPage->isActive())
                    {
                        this->start_timerScrollPage();
                        emit pageNext();
                    }
                }
                else if (event->delta() > 0 && this->boundingRect.y() == 0 && !this->timerScrollPage->isActive())
                {
                    this->start_timerScrollPage();
                    this->flagJumpToEnd = Settings::Instance()->getJumpToEnd();
                    emit pagePrevious();
                }
                else
                {

                    // Keep dragging
                    ScrollPageVertical(event->delta());

                    if ((this->boundingRect.height() + this->boundingRect.y() == this->widgetSize.height()
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
                if (event->delta() > 0)
                {
                    emit pagePrevious();
                }
                else if (event->delta() < 0)
                {
                    emit  pageNext();
                }
            }
        }
    }
}

void PictureItemShared::start_timerScrollPage()
{
    if (Settings::Instance()->getPageChangeTimeout() > 0)
    {
        this->timerScrollPage->start(Settings::Instance()->getPageChangeTimeout());
    }
}

void PictureItemShared::on_timerScrollPage_timeout()
{
    this->timerScrollPage->stop();
}

void PictureItemShared::setPixmapNull(bool value)
{
    this->pixmapNull = value;
}

bool PictureItemShared::isPixmapNull()
{
    return this->pixmapNull;
}

void PictureItemShared::afterPixmapLoad()
{

    this->updateLockMode();

    if (this->boundingRect.width() > this->widgetSize.width())
    {
        if ((Settings::Instance()->getRightToLeft() && !this->flagJumpToEnd)
                || (!Settings::Instance()->getRightToLeft() && this->flagJumpToEnd)

                )
        {
            this->boundingRect.moveLeft(-(this->boundingRect.width() - this->widgetSize.width()));
        }
    }

    if (this->flagJumpToEnd)
    {
        if (this->boundingRect.height() > this->widgetSize.height())
        {
            this->boundingRect.moveTop(-(this->boundingRect.height() - this->widgetSize.height()));
        }
        this->flagJumpToEnd = false;
    }
}
