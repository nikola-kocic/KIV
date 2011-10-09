#include "pictureitem_raster.h"
#include "settings.h"

//#include <QtCore/qdebug.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

PictureItemShared::PictureItemShared()
{
    zoom = 1;
    dragging = false;
    rotation = 0;
    bmp = NULL;
    lockMode = LockMode::None;
    timerScrollPage = new QTimer();
    connect(timerScrollPage, SIGNAL(timeout()), this, SLOT(on_timerScrollPage_timeout()));
//    LockDrag = false;
    flagJumpToEnd = false;
    defaultZoomSizes << 0.1 << 0.25 << 0.5 <<  0.75 << 1 << 1.25 << 1.5 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10;
}

void PictureItemShared::setPixmap(const QPixmap &p)
{
    this->bmp = p;
    pixmapSize = this->bmp.size();

    emit pixmapChanged();
}

QPixmap PictureItemShared::getPixmap()
{
    return bmp;
}

//Region Rotation

void PictureItemShared::setRotation(qreal r)
{
    if(bmp.isNull()) return;

    rotation = r;
}


qreal PictureItemShared::getRotation()
{
    return rotation;
}

//End Region Rotation



//Region Zoom

void PictureItemShared::setZoom(qreal z)
{
    if(bmp.isNull()) return;

    if(z < 0.001) z = 0.001;
    else if(z > 1000) z = 1000;

    qreal previous = zoom;
    zoom = z;

    emit zoomChanged(zoom, previous);
}


QPoint PictureItemShared::pointToOrigin(int width, int height)
{
    qreal zoomX = (qreal)width / (qreal)boundingRect.width();
    qreal zoomY = (qreal)height / (qreal)boundingRect.height();

    if (width > widgetSize.width())
    {
        qreal oldX = (boundingRect.x() - (boundingRect.x() * 2)) + (widgetSize.width() / 2);
        qreal oldY = (boundingRect.y() - (boundingRect.y() * 2)) + (widgetSize.height() / 2);

        qreal newX = oldX * zoomX;
        qreal newY = oldY * zoomY;

        qreal originX = newX - (widgetSize.width() / 2) - ((newX - (widgetSize.width() / 2)) * 2);
        qreal originY = newY - (widgetSize.height() / 2) - ((newY - (widgetSize.height() / 2)) * 2);

        return QPoint(originX, originY);
    }
    else
    {
        if (height > widgetSize.height())
        {
            qreal oldY = (boundingRect.y() - (boundingRect.y() * 2)) + (widgetSize.height() / 2);

            qreal newY = oldY * zoomY;

            qreal originY = newY - (widgetSize.height() / 2) - ((newY - (widgetSize.height() / 2)) * 2);

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
    return zoom;
}

QVector<qreal> PictureItemShared::getDefaultZoomSizes()
{
    return defaultZoomSizes;
}

void PictureItemShared::zoomIn()
{
    for(int i=0; i<defaultZoomSizes.count(); i++)
    {
        if(defaultZoomSizes.at(i) > zoom)
        {
            setZoom(defaultZoomSizes.at(i));
            return;
        }
    }

    setZoom(zoom * 1.25);
}

void PictureItemShared::zoomOut()
{
    for(int i=0; i<defaultZoomSizes.count(); i++)
    {
        if(defaultZoomSizes.at(i) >= zoom)
        {
            if(i != 0)
            {
                setZoom(defaultZoomSizes.at(i-1));
            }
            else
            {
                setZoom(zoom / 1.25);
            }
            return;
        }
    }

    setZoom(zoom / 1.25);
}

void PictureItemShared::fitToScreen()
{
    if(bmp.isNull()) return;

    QRect temp = QRect(boundingRect.x(), boundingRect.y(), boundingRect.width() / zoom, boundingRect.height() / zoom);

    qreal x_ratio = (qreal)widgetSize.width() / temp.width();
    qreal y_ratio = (qreal)widgetSize.height() / temp.height();

    if ((temp.width() <= widgetSize.width()) && (temp.height() <= widgetSize.height()))
    {
        setZoom(1);
    }
    else if ((x_ratio * temp.height()) < widgetSize.height())
    {
        setZoom(x_ratio);
    }
    else
    {
        setZoom(y_ratio);
    }
}

void PictureItemShared::fitWidth()
{
    if(bmp.isNull()) return;

    qreal tw = boundingRect.width() / zoom;

    qreal x_ratio = (qreal)widgetSize.width() / tw;

    if (tw <= widgetSize.width())
    {
        setZoom(1);
    }
    else
    {
        setZoom(x_ratio);
    }
}

void PictureItemShared::fitHeight()
{
    if(bmp.isNull()) return;

    qreal th = boundingRect.height() / zoom;

    qreal y_ratio = (qreal)widgetSize.height() / th;

    if (th <= widgetSize.height())
    {
        setZoom(1);
    }
    else
    {
        setZoom(y_ratio);
    }

}

void PictureItemShared::updateLockMode()
{
    if(bmp.isNull()) return;

    //use in setpixmap and resize events
    switch (lockMode)
    {
    case LockMode::Autofit:
        fitToScreen();
        break;
    case LockMode::FitWidth:
        fitWidth();
        break;
    case LockMode::FitHeight:
        fitHeight();
        break;
    default:
        break;
    }
}

void PictureItemShared::setLockMode(LockMode::Mode mode)
{
    lockMode = mode;

    updateLockMode();
}

LockMode::Mode PictureItemShared::getLockMode()
{
    return lockMode;
}

void PictureItemShared::avoidOutOfScreen()
{
    if(bmp.isNull()) return;

    // Am I lined out to the left?
    if (boundingRect.x() >= 0)
    {
        boundingRect.moveLeft(0);
    }
    else if ((boundingRect.x() <= (boundingRect.width() - widgetSize.width()) - ((boundingRect.width() - widgetSize.width()) * 2)))
    {
        if ((boundingRect.width() - widgetSize.width()) - ((boundingRect.width() - widgetSize.width()) * 2) <= 0)
        {
            // I am too far to the left!
            boundingRect.moveLeft((boundingRect.width() - widgetSize.width()) - ((boundingRect.width() - widgetSize.width()) * 2));
        }
        else
        {
            // I am too far to the right!
            boundingRect.moveLeft(0);
        }
    }

    // Am I lined out to the top?
    if (boundingRect.y() >= 0)
    {
        boundingRect.moveTop(0);
    }
    else if ((boundingRect.y() <= (boundingRect.height() - widgetSize.height()) - ((boundingRect.height() - widgetSize.height()) * 2)))
    {
        if ((boundingRect.height() - widgetSize.height()) - ((boundingRect.height() - widgetSize.height()) * 2) <= 0)
        {
            // I am too far to the top!
            boundingRect.moveTop((boundingRect.height() - widgetSize.height()) - ((boundingRect.height() - widgetSize.height()) * 2));
        }
        else
        {
            // I am too far to the bottom!
            boundingRect.moveTop(0);
        }
    }
}

//End Region Zoom



//Region Drag

void PictureItemShared::drag(const QPoint &pt)
{
    if(bmp.isNull()) return;

    if (dragging == true)
    {
        // Am I dragging it outside of the panel?
        if ((pt.x() - dragPoint.x() >= (boundingRect.width() - widgetSize.width()) - ((boundingRect.width() - widgetSize.width()) * 2)) && (pt.x() - dragPoint.x() <= 0))
        {
            // No, everything is just fine
            boundingRect.moveLeft(pt.x() - dragPoint.x());
        }
        else if ((pt.x() - dragPoint.x() > 0))
        {
            // Now don't drag it out of the panel please
            boundingRect.moveLeft(0);
        }
        else if ((pt.x() - dragPoint.x() < (boundingRect.width() - widgetSize.width()) - ((boundingRect.width() - widgetSize.width()) * 2)))
        {
            // I am dragging it out of my panel. How many pixels do I have left?
            if ((boundingRect.width() - widgetSize.width()) - ((boundingRect.width() - widgetSize.width()) * 2) <= 0)
            {
                // Make it fit perfectly
                boundingRect.moveLeft((boundingRect.width() - widgetSize.width()) - ((boundingRect.width() - widgetSize.width()) * 2));
            }
        }

        // Am I dragging it outside of the panel?
        if (pt.y() - dragPoint.y() >= (boundingRect.height() - widgetSize.height()) - ((boundingRect.height() - widgetSize.height()) * 2) && (pt.y() - dragPoint.y() <= 0))
        {
            // No, everything is just fine
            boundingRect.moveTop(pt.y() - dragPoint.y());
        }
        else if ((pt.y() - dragPoint.y() > 0))
        {
            // Now don't drag it out of the panel please
            boundingRect.moveTop(0);
        }
        else if (pt.y() - dragPoint.y() < (boundingRect.height() - widgetSize.height()) - ((boundingRect.height() - widgetSize.height()) * 2))
        {
            // I am dragging it out of my panel. How many pixels do I have left?
            if ((boundingRect.height() - widgetSize.height()) - ((boundingRect.height() - widgetSize.height()) * 2) <= 0)
            {
                // Make it fit perfectly
                boundingRect.moveTop((boundingRect.height() - widgetSize.height()) - ((boundingRect.height()- widgetSize.height()) * 2));
            }
        }
        emit update();
    }
}


void PictureItemShared::processMousePressEvent(QMouseEvent *ev)
{
    if (ev->button() ==  Qt::RightButton)
    {
        if(ev->buttons() == (Qt::LeftButton | Qt::RightButton))
        {
            emit pageNext();
//            _HandledPageChange = true;
        }
    }
    else if (ev->button() == Qt::LeftButton
//             || LockDrag == true
             )
    {
        if(ev->buttons() == (Qt::LeftButton | Qt::RightButton))
        {
            emit pagePrevious();
//            _HandledPageChange = true;
        }
        else
        {

            // Start dragging
            beginDrag(ev->pos());
        }
    }
    else if(ev->button() == Qt::MiddleButton)
    {
        switch(Settings::Instance()->getMiddleClick())
        {
        case MiddleClick::Fullscreen :
            emit(toggleFullscreen());
            break;

        case MiddleClick::AutoFit:
            fitToScreen();
            break;

        case MiddleClick::ZoomReset:
            setZoom(1);
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
    if(bmp.isNull()) return;

    // Initial drag position
    dragPoint.setX(pt.x() - boundingRect.x());
    dragPoint.setY(pt.y() - boundingRect.y());
    dragging = true;
    emit updateCursor(Qt::ClosedHandCursor);
}

void PictureItemShared::endDrag()
{
    if(bmp.isNull()) return;

    dragging = false;
    emit updateCursor(Qt::OpenHandCursor);
}

//End Region Drag

void PictureItemShared::ScrollPageVertical(int value)
{
    beginDrag(QPoint(0,0));
    drag(QPoint(0,value));
    endDrag();
}

void PictureItemShared::ScrollPageHorizontal(int value)
{
    beginDrag(QPoint(0,0));
    drag(QPoint(value,0));
    endDrag();
}

bool PictureItemShared::processKeyPressEvent(int key)
{
    if(key == Qt::Key_Up)
    {
        ScrollPageVertical(120);
        return true;
    }
    else if(key == Qt::Key_Down)
    {
        ScrollPageVertical(-120);
        return true;
    }
    else if(key == Qt::Key_Left)
    {
        ScrollPageHorizontal(120);
        return true;
    }
    else if(key == Qt::Key_Right)
    {
        ScrollPageHorizontal(-120);
        return true;
    }

    return false;
}

void PictureItemShared::processWheelEvent( QWheelEvent *event )
{
    if(event->modifiers() == Qt::ControlModifier
            || (event->modifiers() == Qt::NoModifier && Settings::Instance()->getWheel() == Wheel::Zoom)
            )
    {
        if(event->delta() < 0)
        {
            zoomOut();
        }
        else
        {
            zoomIn();
        }
    }
    else if(event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
    {
        setZoom(zoom * (1 + ((event->delta() / 4.8) / 100)));
    }
    else if(event->modifiers() == Qt::ShiftModifier)
    {
        ScrollPageVertical(event->delta());
    }
    else if(event->modifiers() == Qt::AltModifier)
    {
        ScrollPageHorizontal(event->delta());
    }
    else if(event->modifiers() == Qt::NoModifier)
    {
        if(Settings::Instance()->getWheel() == Wheel::ChangePage)
        {
            if(event->delta() < 0)
            {
                emit pageNext();
            }
            else
            {
                emit pagePrevious();
            }
        }
        else if(Settings::Instance()->getWheel() == Wheel::Scroll)
        {
            if((Settings::Instance()->getPageChangeTimeout() > 0) && (boundingRect.height() > widgetSize.height() || boundingRect.width() > widgetSize.width()))
            {
                //If we scroll to bottom of page, start timer
                if (event->delta() < 0 && -boundingRect.y() + widgetSize.height() >= boundingRect.height() && timerScrollPage->isActive() == false)
                {
                    if (Settings::Instance()->getScrollPageByWidth() == true)
                    {
                        if(Settings::Instance()->getRightToLeft() == true)
                        {
                            if(boundingRect.x() < 0)
                            {
                                ScrollPageHorizontal(-event->delta());

                                if(boundingRect.x() == 0)
                                {
                                    start_timerScrollPage();
                                }
                                return;
                            }
                        }
                        else
                        {
                            if((boundingRect.width() + boundingRect.x()) > widgetSize.width() )
                            {
                                ScrollPageHorizontal(event->delta());


                                if(boundingRect.width() + boundingRect.x() == widgetSize.width())
                                {
                                    start_timerScrollPage();
                                }
                                return;
                            }
                        }
                    }
                    if(timerScrollPage->isActive() == false)
                    {
                        start_timerScrollPage();
                        emit pageNext();
                    }
                }
                else if (event->delta() > 0 && boundingRect.y() == 0 && timerScrollPage->isActive() == false)
                {
                    start_timerScrollPage();
                    flagJumpToEnd = Settings::Instance()->getJumpToEnd();
                    emit pagePrevious();
                }
                else
                {

                    // Keep dragging
                    ScrollPageVertical(event->delta());

                    if((boundingRect.height() + boundingRect.y() == widgetSize.height()
                        || boundingRect.y() == 0)
                            && timerScrollPage->isActive() == false
                            )
                    {
                        start_timerScrollPage();
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
    if(Settings::Instance()->getPageChangeTimeout() > 0)
    {
        timerScrollPage->start(Settings::Instance()->getPageChangeTimeout());
    }
}

void PictureItemShared::on_timerScrollPage_timeout()
{
    timerScrollPage->stop();
}

bool PictureItemShared::isPixmapNull()
{
    return this->bmp.isNull();
}

void PictureItemShared::afterPixmapLoad()
{

    updateLockMode();

    if(boundingRect.width() > widgetSize.width())
    {
        if((Settings::Instance()->getRightToLeft() == true && flagJumpToEnd == false)
                || (Settings::Instance()->getRightToLeft() == false && flagJumpToEnd == true)

                )
        {
            boundingRect.moveLeft(-(boundingRect.width() - widgetSize.width()));
        }
    }

    if(flagJumpToEnd == true)
    {
        if(boundingRect.height() > widgetSize.height())
        {
            boundingRect.moveTop(-(boundingRect.height() - widgetSize.height()));
        }
        flagJumpToEnd = false;
    }
}

QSize PictureItemShared::getPixmapSize()
{
    return this->pixmapSize;
}
