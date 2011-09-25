#include <QtGui/qpixmap.h>
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>
#include "KomicViewer.h"
#include "settings.h"
#include <QtCore/qdebug.h>

PictureItem::PictureItem(QWidget * parent, Qt::WindowFlags f )
{
    zoom = 1;
    dragging = false;
    rotation = 0;
    bmp = NULL;
    setCursor(Qt::OpenHandCursor);
    lockMode = LockMode::None;
    timerScrollPage = new QTimer();
    connect(timerScrollPage, SIGNAL(timeout()), this, SLOT(on_timerScrollPage_timeout()));
//    LockDrag = false;
    flagJumpToEnd = false;
    defaultZoomSizes << 0.1 << 0.25 << 0.5 <<  0.75 << 1 << 1.25 << 1.5 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10;
}

void PictureItem::setPixmap(const QPixmap &p)
{
    //    if (p.isNull()==false)
    //    {
    this->bmp = p;

    this->pixmap_edited = p;
    boundingRect = QRect(0, 0, p.width(), p.height());
    //    setRotation(0);
    //    setZoom(1);
    if(lockMode != LockMode::Zoom)
    {
	zoom = 1.0;
    }
    rotation = 0.0;
    emit zoomChanged();

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

//    editPixmap();

    update();
    //    }
}

void PictureItem::ScrollPageVertical(int value)
{
    beginDrag(QPoint(0,0));
    drag(QPoint(0,value));
    endDrag();
}

void PictureItem::ScrollPageHorizontal(int value)
{
    beginDrag(QPoint(0,0));
    drag(QPoint(value,0));
    endDrag();
}

void PictureItem::keyPressEvent(QKeyEvent *ev)
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

QPixmap PictureItem::getPixmap()
{
    return bmp;
}

void PictureItem::paintEvent(QPaintEvent *event)
{
    if(bmp.isNull()) return;

    QTime myTimer;
    myTimer.start();

    QPainter p(this);
    p.setClipRect(event->region().boundingRect());
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    QRect sourceRect = QRect(-boundingRect.x() / zoom, -boundingRect.y() / zoom, event->region().boundingRect().width() / zoom, event->region().boundingRect().height() / zoom);

    int x = event->region().boundingRect().x();
    int y = event->region().boundingRect().y();
    int w = event->region().boundingRect().width();
    int h = event->region().boundingRect().height();

    if(w  > (pixmap_edited.width() * zoom)) x = (w - (pixmap_edited.width() * zoom)) / 2;
    if(h  > (pixmap_edited.height() * zoom)) y = (h - (pixmap_edited.height() * zoom)) / 2;

    QRect drawRect = QRect(x, y, w, h);

    p.drawPixmap(drawRect,pixmap_edited, sourceRect);

    p.end();

//    qDebug() << "Paint: " << myTimer.elapsed();
}


//Region Rotation

void PictureItem::setRotation(qreal r)
{
    if(bmp.isNull()) return;

    QTime myTimer;
    myTimer.start();

    rotation = r;

    QTransform tRot;
    tRot.rotate(rotation);

    Qt::TransformationMode rotateMode;
    rotateMode = Qt::SmoothTransformation;
//    rotateMode = Qt::FastTransformation;

    pixmap_edited = bmp.transformed(tRot, rotateMode);

    boundingRect.setWidth(pixmap_edited.width() * zoom);
    boundingRect.setHeight(pixmap_edited.height()  * zoom);

    avoidOutOfScreen();
    update();

//    qDebug() << "Rotate: " << myTimer.elapsed();

}


qreal PictureItem::getRotation()
{
    return rotation;
}

//End Region Rotation



//Region Zoom

void PictureItem::setZoom(qreal z)
{
    if(bmp.isNull()) return;

    if(z < 0.001) z = 0.001;
    else if(z > 1000) z = 1000;

    QPoint p = pointToOrigin((int)(this->pixmap_edited.width() * z), (int)(this->pixmap_edited.height() * z));
    boundingRect = QRect(p.x(), p.y(), (int)(pixmap_edited.width() * z), (int)(pixmap_edited.height() * z));

    zoom = z;

    avoidOutOfScreen();
    update();

    emit zoomChanged();
}


QPoint PictureItem::pointToOrigin(int width, int height)
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

qreal PictureItem::getZoom()
{
    return zoom;
}

QVector<qreal> PictureItem::getDefaultZoomSizes()
{
    return defaultZoomSizes;
}

void PictureItem::zoomIn()
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

void PictureItem::zoomOut()
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

void PictureItem::fitToScreen()
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

void PictureItem::fitWidth()
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

void PictureItem::fitHeight()
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

void PictureItem::updateLockMode()
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

void PictureItem::setLockMode(LockMode::Mode mode)
{
    lockMode = mode;

    updateLockMode();
}

LockMode::Mode PictureItem::getLockMode()
{
    return lockMode;
}

void PictureItem::avoidOutOfScreen()
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

void PictureItem::drag(const QPoint &pt)
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


void PictureItem::mousePressEvent(QMouseEvent *ev)
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

void PictureItem::mouseMoveEvent(QMouseEvent *ev)
{
    if(dragging == true)
    {
        drag(ev->pos());
    }
}

void PictureItem::mouseReleaseEvent(QMouseEvent *ev)
{
    if(dragging == true && ev->button() == Qt::LeftButton)
    {
        endDrag();
    }
}

void PictureItem::beginDrag(const QPoint &pt)
{
    if(bmp.isNull()) return;

    // Initial drag position
    dragPoint.setX(pt.x() - boundingRect.x());
    dragPoint.setY(pt.y() - boundingRect.y());
    dragging = true;
    setCursor(Qt::ClosedHandCursor);

}

void PictureItem::endDrag()
{
    if(bmp.isNull()) return;

    dragging = false;
    setCursor(Qt::OpenHandCursor);
}

//End Region Drag





void PictureItem::resizeEvent(QResizeEvent *)
{
    if(bmp.isNull()) return;

    avoidOutOfScreen();
    updateLockMode();
}

void PictureItem::wheelEvent( QWheelEvent *event )
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

void PictureItem::start_timerScrollPage()
{
    if(Settings::Instance()->getPageChangeTimeout() > 0)
    {
        timerScrollPage->start(Settings::Instance()->getPageChangeTimeout());
    }
}

void PictureItem::on_timerScrollPage_timeout()
{
    timerScrollPage->stop();
}

