#include "pictureitem_gl.h"
#include "settings.h"

//#include <QtCore/qdebug.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

PictureItemGL::PictureItemGL(QWidget * parent, Qt::WindowFlags f )
{
    zoom = 1;
    dragging = false;
    rotation = 0;
    setCursor(Qt::OpenHandCursor);
    lockMode = LockMode::None;
    timerScrollPage = new QTimer();
    connect(timerScrollPage, SIGNAL(timeout()), this, SLOT(on_timerScrollPage_timeout()));
//    LockDrag = false;
    flagJumpToEnd = false;
    defaultZoomSizes << 0.1 << 0.25 << 0.5 <<  0.75 << 1 << 1.25 << 1.5 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10;
}

void PictureItemGL::setPixmap(const QPixmap &p)
{
    //    if (p.isNull()=) return;

    this->bmp = p;

    setRotation(0);
    if(lockMode != LockMode::Zoom)
    {
        setZoom(1);
    }
    boundingRect = QRect(0, 0, p.width() * zoom, p.height() * zoom);

    updateLockMode();

    if(boundingRect.width() > this->width())
    {
        if((Settings::Instance()->getRightToLeft() == true && flagJumpToEnd == false)
                || (Settings::Instance()->getRightToLeft() == false && flagJumpToEnd == true)

                )
        {
            boundingRect.moveLeft(-(boundingRect.width() - this->width()));
        }
    }

    if(flagJumpToEnd == true)
    {
        if(boundingRect.height() > this->height())
        {
            boundingRect.moveTop(-(boundingRect.height() - this->height()));
        }
        flagJumpToEnd = false;
    }

    update();
}

QPixmap PictureItemGL::getPixmap()
{
    return bmp;
}

void PictureItemGL::paintEvent(QPaintEvent *event)
{
    if(bmp.isNull()) return;

    QPainter p(this);
    p.setClipRect(event->region().boundingRect());

    int x = event->region().boundingRect().x();
    int y = event->region().boundingRect().y();
    int w = event->region().boundingRect().width();
    int h = event->region().boundingRect().height();

    if(w  > boundingRect.width()) x = (w - boundingRect.width()) / 2;
    if(h  > boundingRect.height()) y = (h - boundingRect.height()) / 2;

//	qDebug() << x << y;
//	qDebug()<<"zoom" << zoom << "boundingRect: " << boundingRect<< "this size" << this->size();

    p.setRenderHint(QPainter::SmoothPixmapTransform);

    p.translate(boundingRect.x() + translatePoint.x() + x, boundingRect.y() + translatePoint.y() + y);
    p.scale(zoom,zoom);
    p.translate(bmp.height()/2, bmp.width()/2);
    p.rotate(rotation);
    p.translate(-bmp.height()/2, -bmp.width()/2);

    p.drawPixmap(0,0,bmp);
    p.end();
}


//Region Rotation

void PictureItemGL::setRotation(qreal r)
{
    if(bmp.isNull()) return;

    rotation = r;

    QTransform tRot;
    tRot.translate(boundingRect.x(), boundingRect.y());
    tRot.scale(zoom,zoom);
    tRot.translate(bmp.height()/2, bmp.width()/2);
    tRot.rotate(rotation);
    tRot.translate(-bmp.height()/2, -bmp.width()/2);
    QRect transformedRot = tRot.mapRect(bmp.rect());

    translatePoint = QPoint(boundingRect.x() - transformedRot.x(), boundingRect.y() - transformedRot.y());
    boundingRect.setWidth(transformedRot.width());
    boundingRect.setHeight(transformedRot.height());

    if(boundingRect.height() + boundingRect.y() < this->height())
    {
        boundingRect.translate(0, this->height() - (boundingRect.height() + boundingRect.y()));
    }

    if(boundingRect.height()  < this->height())
    {
        boundingRect.moveTop(0);
    }

    update();
}


qreal PictureItemGL::getRotation()
{
    return rotation;
}

//End Region Rotation



//Region Zoom

void PictureItemGL::setZoom(qreal z)
{
    if (this->bmp.isNull()==false)
    {
        if(z < 0.001) z = 0.001;
        else if(z > 1000) z = 1000;
        QPoint p = pointToOrigin((boundingRect.width() / zoom) * z, (boundingRect.height() / zoom) * z);
        boundingRect = QRect(p.x(), p.y(), (boundingRect.width() / zoom) * z, (boundingRect.height() / zoom) * z);

        zoom = z;

        setRotation(rotation);
        avoidOutOfScreen();

        update();

        emit zoomChanged();
    }
}

QPoint PictureItemGL::pointToOrigin(int width, int height)
{
    qreal zoomX = (qreal)width / (qreal)boundingRect.width();
    qreal zoomY = (qreal)height / (qreal)boundingRect.height();

    if (width > this->width())
    {
        qreal oldX = (boundingRect.x() - (boundingRect.x() * 2)) + (this->width() / 2);
        qreal oldY = (boundingRect.y() - (boundingRect.y() * 2)) + (this->height() / 2);

        qreal newX = oldX * zoomX;
        qreal newY = oldY * zoomY;

        qreal originX = newX - (this->width() / 2) - ((newX - (this->width() / 2)) * 2);
        qreal originY = newY - (this->height() / 2) - ((newY - (this->height() / 2)) * 2);

        return QPoint(originX, originY);
    }
    else
    {
        if (height > this->height())
        {
            qreal oldY = (boundingRect.y() - (boundingRect.y() * 2)) + (this->height() / 2);

            qreal newY = oldY * zoomY;

            qreal originY = newY - (this->height() / 2) - ((newY - (this->height() / 2)) * 2);

            return QPoint(0, originY);
        }
        else
        {
            return QPoint(0, 0);
        }
    }
}

qreal PictureItemGL::getZoom()
{
    return zoom;
}

QVector<qreal> PictureItemGL::getDefaultZoomSizes()
{
    return defaultZoomSizes;
}

void PictureItemGL::zoomIn()
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

void PictureItemGL::zoomOut()
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

void PictureItemGL::fitToScreen()
{
    if(bmp.isNull()) return;

    QRect temp = QRect(boundingRect.x(), boundingRect.y(), boundingRect.width() / zoom, boundingRect.height() / zoom);

    qreal x_ratio = (qreal)this->width() / temp.width();
    qreal y_ratio = (qreal)this->height() / temp.height();

    if ((temp.width() <= this->width()) && (temp.height() <= this->height()))
    {
        setZoom(1);
    }
    else if ((x_ratio * temp.height()) < this->height())
    {
        setZoom(x_ratio);
    }
    else
    {
        setZoom(y_ratio);
    }
}

void PictureItemGL::fitWidth()
{
    if(bmp.isNull()) return;

    qreal tw = boundingRect.width() / zoom;

    qreal x_ratio = (qreal)this->width() / tw;

    if (tw <= this->width())
    {
        setZoom(1);
    }
    else
    {
        setZoom(x_ratio);
    }
}

void PictureItemGL::fitHeight()
{
    if(bmp.isNull()) return;

    qreal th = boundingRect.height() / zoom;

    qreal y_ratio = (qreal)this->height() / th;

    if (th <= this->height())
    {
        setZoom(1);
    }
    else
    {
        setZoom(y_ratio);
    }

}

void PictureItemGL::updateLockMode()
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

void PictureItemGL::setLockMode(LockMode::Mode mode)
{
    lockMode = mode;

    updateLockMode();
}

LockMode::Mode PictureItemGL::getLockMode()
{
    return lockMode;
}

void PictureItemGL::avoidOutOfScreen()
{
    if(bmp.isNull()) return;

    // Am I lined out to the left?
    if (boundingRect.x() >= 0)
    {
        boundingRect.moveLeft(0);
    }
    else if ((boundingRect.x() <= (boundingRect.width() - this->width()) - ((boundingRect.width() - this->width()) * 2)))
    {
        if ((boundingRect.width() - this->width()) - ((boundingRect.width() - this->width()) * 2) <= 0)
        {
            // I am too far to the left!
            boundingRect.moveLeft((boundingRect.width() - this->width()) - ((boundingRect.width() - this->width()) * 2));
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
    else if ((boundingRect.y() <= (boundingRect.height() - this->height()) - ((boundingRect.height() - this->height()) * 2)))
    {
        if ((boundingRect.height() - this->height()) - ((boundingRect.height() - this->height()) * 2) <= 0)
        {
            // I am too far to the top!
            boundingRect.moveTop((boundingRect.height() - this->height()) - ((boundingRect.height() - this->height()) * 2));
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

void PictureItemGL::drag(const QPoint &pt)
{
    if(bmp.isNull()) return;

    if (dragging == true)
    {
        // Am I dragging it outside of the panel?
        if ((pt.x() - dragPoint.x() >= (boundingRect.width() - this->width()) - ((boundingRect.width() - this->width()) * 2)) && (pt.x() - dragPoint.x() <= 0))
        {
            // No, everything is just fine
            boundingRect.moveLeft(pt.x() - dragPoint.x());
        }
        else if ((pt.x() - dragPoint.x() > 0))
        {
            // Now don't drag it out of the panel please
            boundingRect.moveLeft(0);
        }
        else if ((pt.x() - dragPoint.x() < (boundingRect.width() - this->width()) - ((boundingRect.width() - this->width()) * 2)))
        {
            // I am dragging it out of my panel. How many pixels do I have left?
            if ((boundingRect.width() - this->width()) - ((boundingRect.width() - this->width()) * 2) <= 0)
            {
                // Make it fit perfectly
                boundingRect.moveLeft((boundingRect.width() - this->width()) - ((boundingRect.width() - this->width()) * 2));
            }
        }

        // Am I dragging it outside of the panel?
        if (pt.y() - dragPoint.y() >= (boundingRect.height() - this->height()) - ((boundingRect.height() - this->height()) * 2) && (pt.y() - dragPoint.y() <= 0))
        {
            // No, everything is just fine
            boundingRect.moveTop(pt.y() - dragPoint.y());
        }
        else if ((pt.y() - dragPoint.y() > 0))
        {
            // Now don't drag it out of the panel please
            boundingRect.moveTop(0);
        }
        else if (pt.y() - dragPoint.y() < (boundingRect.height() - this->height()) - ((boundingRect.height() - this->height()) * 2))
        {
            // I am dragging it out of my panel. How many pixels do I have left?
            if ((boundingRect.height() - this->height()) - ((boundingRect.height() - this->height()) * 2) <= 0)
            {
                // Make it fit perfectly
                boundingRect.moveTop((boundingRect.height() - this->height()) - ((boundingRect.height()- this->height()) * 2));
            }
        }
        update();
    }
}


void PictureItemGL::mousePressEvent(QMouseEvent *ev)
{
    setFocus();


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

void PictureItemGL::mouseMoveEvent(QMouseEvent *ev)
{
    if(dragging == true)
    {
        drag(ev->pos());
    }
}

void PictureItemGL::mouseReleaseEvent(QMouseEvent *ev)
{
    if(dragging == true && ev->button() == Qt::LeftButton)
    {
        endDrag();
    }
}

void PictureItemGL::beginDrag(const QPoint &pt)
{
    if(bmp.isNull()) return;

    // Initial drag position
    dragPoint.setX(pt.x() - boundingRect.x());
    dragPoint.setY(pt.y() - boundingRect.y());
    dragging = true;
    setCursor(Qt::ClosedHandCursor);

}

void PictureItemGL::endDrag()
{

    if(bmp.isNull()) return;

    dragging = false;
    setCursor(Qt::OpenHandCursor);
}

//End Region Drag





void PictureItemGL::resizeEvent(QResizeEvent *)
{
    if(bmp.isNull()) return;

    avoidOutOfScreen();
    updateLockMode();
}


void PictureItemGL::ScrollPageVertical(int value)
{
    beginDrag(QPoint(0,0));
    drag(QPoint(0,value));
    endDrag();
}

void PictureItemGL::ScrollPageHorizontal(int value)
{
    beginDrag(QPoint(0,0));
    drag(QPoint(value,0));
    endDrag();
}

void PictureItemGL::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Up)
    {
        ScrollPageVertical(120);
        ev->accept();
    }
    else if(ev->key() == Qt::Key_Down)
    {
        ScrollPageVertical(-120);
        ev->accept();
    }
    else if(ev->key() == Qt::Key_Left)
    {
        ScrollPageHorizontal(120);
        ev->accept();
    }
    else if(ev->key() == Qt::Key_Right)
    {
        ScrollPageHorizontal(-120);
        ev->accept();
    }
}

void PictureItemGL::wheelEvent( QWheelEvent *event )
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
            if((Settings::Instance()->getPageChangeTimeout() > 0) && (boundingRect.height() > this->height() || boundingRect.width() > this->width()))
            {
                //If we scroll to bottom of page, start timer
                if (event->delta() < 0 && -boundingRect.y() + this->height() >= boundingRect.height() && timerScrollPage->isActive() == false)
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
                            if((boundingRect.width() + boundingRect.x()) > this->width() )
                            {
                                ScrollPageHorizontal(event->delta());


                                if(boundingRect.width() + boundingRect.x() == this->width())
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

                    if((boundingRect.height() + boundingRect.y() == this->height()
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

void PictureItemGL::start_timerScrollPage()
{
    if(Settings::Instance()->getPageChangeTimeout() > 0)
    {
        timerScrollPage->start(Settings::Instance()->getPageChangeTimeout());
    }
}

void PictureItemGL::on_timerScrollPage_timeout()
{
    timerScrollPage->stop();
}

