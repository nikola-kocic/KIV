#include "pictureitem.h"
#include "settings.h"

#include <QMouseEvent>

PictureItem::PictureItem(bool opengl, QWidget *parent, Qt::WindowFlags f) : QWidget(parent)
{
    this->setCursor(Qt::OpenHandCursor);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    m_opengl = opengl;
    m_imageDisplay_raster = 0;
    m_imageDisplay_gl = 0;
    m_defaultZoomSizes << 0.1 << 0.25 << 0.5 <<  0.75 << 1.0 << 1.25 << 1.5 << 2.0 << 3.0 << 4.0 << 5.0 << 6.0 << 7.0 << 8.0 << 9.0 << 10.0;
    m_zoom_value = 1.0;
    m_rotation_value = 0.0;
    m_dragging = false;
    m_flag_jumpToEnd = false;
    m_lockMode = LockMode::None;

    m_timer_scrollPage = new QTimer(this);
    connect(m_timer_scrollPage, SIGNAL(timeout()), m_timer_scrollPage, SLOT(stop()));

    m_loader_texture = new QFutureWatcher<QImage>(this);
    connect(m_loader_texture, SIGNAL(resultReadyAt(int)), this, SLOT(textureFinished(int)));

    m_loader_image = new QFutureWatcher<QImage>(this);
    connect(m_loader_image, SIGNAL(resultReadyAt(int)), this, SLOT(imageFinished(int)));

    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setSpacing(0);
    layoutMain->setMargin(0);
    this->setLayout(layoutMain);

    initPictureItem();
}

void PictureItem::initPictureItem()
{
    if (m_opengl)
    {
        m_imageDisplay_gl = new PictureItemGL(this);
        m_imageDisplay_raster = 0;
        this->layout()->addWidget(m_imageDisplay_gl);
    }
    else
    {
        m_imageDisplay_raster = new PictureItemRaster(this);
        m_imageDisplay_gl = 0;
        this->layout()->addWidget(m_imageDisplay_raster);
    }
}

bool PictureItem::getHardwareAcceleration() const
{
    return m_opengl;
}

void PictureItem::setHardwareAcceleration(bool b)
{
    if (m_opengl != b)
    {
        if (m_opengl)
        {
            m_imageDisplay_gl->disconnect();
            m_imageDisplay_gl->deleteLater();
        }
        else
        {
            m_imageDisplay_raster->disconnect();
            m_imageDisplay_raster->deleteLater();
        }

        m_opengl = b;
        initPictureItem();
    }
}


bool PictureItem::isPixmapNull() const
{
    return m_pixmapNull;
}

void PictureItem::setPixmapNull(bool value)
{
    m_pixmapNull = value;
}


qreal PictureItem::getRotation() const
{
    return m_rotation_value;
}

void PictureItem::setRotation(qreal r)
{
    if (isPixmapNull())
    {
        return;
    }

    m_rotation_value = r;

    if (m_opengl)
    {
        m_imageDisplay_gl->setRotation(r);
    }
    else
    {
        m_imageDisplay_raster->setRotation(r);
    }
}


qreal PictureItem::getZoom()
{
    return m_zoom_value;
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

    qreal previous = m_zoom_value;
    m_zoom_value = z;

    if (m_opengl)
    {
        m_imageDisplay_gl->setZoom(m_zoom_value, previous);
    }
    else
    {
        m_imageDisplay_raster->setZoom(m_zoom_value, previous);
    }

    emit zoomChanged(z, previous);
}


LockMode::Mode PictureItem::getLockMode() const
{
    return m_lockMode;
}

void PictureItem::setLockMode(LockMode::Mode mode)
{
    m_lockMode = mode;
    this->updateLockMode();
}


QList<qreal> PictureItem::getDefaultZoomSizes() const
{
    return m_defaultZoomSizes;
}









void PictureItem::setPixmap(const FileInfo &info)
{
#ifdef DEBUG_PICTUREITEM
    qDebug() << QDateTime::currentDateTime() << "PictureItem::setPixmap" << info.getFilePath();
#endif
    m_returnTexCount = 0;
    if (!info.fileExists())
    {
        if (m_opengl)
        {
            m_imageDisplay_gl->setImage(QImage());
        }
        else
        {
            m_imageDisplay_raster->setImage(QImage());
        }
        this->setPixmapNull(true);
        emit imageChanged();
    }
    else
    {
#ifdef DEBUG_PICTUREITEM
        t.start();
#endif
        m_loader_image->setFuture(QtConcurrent::run(PictureLoader::getImage, info));
    }
}

void PictureItem::imageFinished(int num)
{
#ifdef DEBUG_PICTUREITEM
    qDebug() << QDateTime::currentDateTime() << "\nloaded image" << t.restart();
#endif
    this->setPixmapNull(m_loader_image->resultAt(num).isNull());
    if (m_opengl)
    {
        m_imageDisplay_gl->setImage(m_loader_image->resultAt(num));
    }
    else
    {
        m_imageDisplay_raster->setImage(m_loader_image->resultAt(num));
        emit imageChanged();
    }

    /* Free result memory */
    m_loader_image->setFuture(QFuture<QImage>());
}

void PictureItem::loadTextures(QList<TexIndex> indexes)
{
    m_returnTexCount = indexes.size();
    m_loader_texture->setFuture(QtConcurrent::mapped(indexes, TexImg::CreatePow2Bitmap));
}

void PictureItem::textureFinished(int num)
{
    if (m_opengl)
    {
        m_imageDisplay_gl->setTexture(m_loader_texture->resultAt(num), num);
        if (--m_returnTexCount == 0)
        {
            this->setPixmapNull(false);
            m_loader_texture->setFuture(QFuture<QImage>());
            m_imageDisplay_gl->textureLoadFinished();

#ifdef DEBUG_PICTUREITEM
            qDebug() << QDateTime::currentDateTime() << "loaded textures" << t.elapsed();
#endif
            emit imageChanged();
        }
    }
}

void PictureItem::afterPixmapLoad()
{
    this->updateLockMode();

    if (m_boundingRect.width() > this->size().width())
    {
        if ((Settings::Instance()->getRightToLeft() && !m_flag_jumpToEnd)
                || (!Settings::Instance()->getRightToLeft() && m_flag_jumpToEnd)

                )
        {
            m_boundingRect.moveLeft(-(m_boundingRect.width() - this->size().width()));
        }
    }

    if (m_flag_jumpToEnd)
    {
        if (m_boundingRect.height() > this->size().height())
        {
            m_boundingRect.moveTop(-(m_boundingRect.height() - this->size().height()));
        }
        m_flag_jumpToEnd = false;
    }
}





void PictureItem::mousePressEvent(QMouseEvent *ev)
{
    this->setFocus();

    if (ev->button() ==  Qt::RightButton)
    {
        if (this->contextMenuPolicy() == Qt::PreventContextMenu)
        {
            this->setContextMenuPolicy(Qt::CustomContextMenu);
        }
        if (ev->buttons() == (Qt::LeftButton | Qt::RightButton))
        {
            this->setContextMenuPolicy(Qt::PreventContextMenu);
            emit pageNext();
            ev->accept();
        }
    }
    else if (ev->button() == Qt::LeftButton)
    {
        if (ev->buttons() == (Qt::LeftButton | Qt::RightButton))
        {
            this->setContextMenuPolicy(Qt::PreventContextMenu);
            emit pagePrevious();
            ev->accept();
        }
        else
        {

            /* Start dragging */
            this->beginDrag(ev->pos());
        }
    }
    else if (ev->button() == Qt::MiddleButton)
    {
        switch (Settings::Instance()->getMiddleClick())
        {
        case MiddleClick::Fullscreen :
            emit(toggleFullscreen());
            ev->accept();
            break;

        case MiddleClick::AutoFit:
            this->fitToScreen();
            ev->accept();
            break;

        case MiddleClick::ZoomReset:
            this->setZoom(1);
            ev->accept();
            break;

        case MiddleClick::NextPage:
            emit pageNext();
            ev->accept();
            break;

        case MiddleClick::Quit:
            emit quit();
            ev->accept();
            break;

        case MiddleClick::Boss:
            emit boss();
            ev->accept();
            break;

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
    if (m_dragging && ev->button() == Qt::LeftButton)
    {
        this->endDrag();
    }
}

/* End Region Drag */


void PictureItem::mouseDoubleClickEvent(QMouseEvent *ev)
{
    if (ev->buttons() == Qt::LeftButton)
    {
        emit(toggleFullscreen());
        ev->accept();
    }
}

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
    switch (ev->key())
    {
    case Qt::Key_Up:
        this->ScrollPageVertical(120);
        ev->accept();
        break;

    case Qt::Key_Down:
        this->ScrollPageVertical(-120);
        ev->accept();
        break;

    case Qt::Key_Left:
        this->ScrollPageHorizontal(120);
        ev->accept();
        break;

    case Qt::Key_Right:
        this->ScrollPageHorizontal(-120);
        ev->accept();
        break;

    case Qt::Key_B:
        emit boss();
        ev->accept();
        break;

    case Qt::Key_Escape:
        emit setFullscreen(false);
        ev->accept();
        break;

    case Qt::Key_Return:
        if (ev->modifiers() & Qt::AltModifier)
        {
            emit toggleFullscreen();
            ev->accept();
        }
        break;

    case Qt::Key_Menu:
        if (this->contextMenuPolicy() == Qt::PreventContextMenu)
        {
            this->setContextMenuPolicy(Qt::CustomContextMenu);
        }
    }
}

void PictureItem::wheelEvent(QWheelEvent *event)
{
    /* event->delta() > 0 == Up
       event->delta() < 0 == Down */

    if (Qt::NoModifier == event->modifiers())
    {
        /* If page can't be scrolled, change page if necessary */
        if (Wheel::ChangePage == Settings::Instance()->getWheel() ||
                (
                    (Wheel::Scroll == Settings::Instance()->getWheel()) &&
                    (Settings::Instance()->getScrollChangesPage()) &&
                    (
                        (LockMode::Autofit == m_lockMode) ||
                        (LockMode::FitHeight == m_lockMode) ||
                        (m_boundingRect.height() <= this->size().height() && m_boundingRect.width() <= this->size().width())
                        )
                    )
                )
        {
            if (event->delta() < 0)
            {
                emit pageNext();
                event->accept();
            }
            else
            {
                m_flag_jumpToEnd = Settings::Instance()->getJumpToEnd();
                emit pagePrevious();
                event->accept();
            }
        }
        /* Scroll page */
        else if (Wheel::Scroll == Settings::Instance()->getWheel())
        {
            event->accept();
            if (
                    (event->delta() < 0) &&
                    (!m_timer_scrollPage->isActive()) &&
                    ((-m_boundingRect.y() + this->size().height()) >= m_boundingRect.height())
                    )
            {
                /* Scroll horizontally; If page is scrolled to end, start timer */
                if (Settings::Instance()->getScrollPageByWidth())
                {
                    if (Settings::Instance()->getRightToLeft())
                    {
                        if (m_boundingRect.x() < 0)
                        {
                            this->ScrollPageHorizontal(-event->delta());

                            if (0 == m_boundingRect.x())
                            {
                                this->start_timerScrollPage();
                            }
                            return;
                        }
                    }
                    else
                    {
                        if ((m_boundingRect.width() + m_boundingRect.x()) > this->size().width())
                        {
                            this->ScrollPageHorizontal(event->delta());


                            if ((m_boundingRect.width() + m_boundingRect.x()) == this->size().width())
                            {
                                this->start_timerScrollPage();
                            }
                            return;
                        }
                    }
                }
                /* If this code is reached (page was already scrolled to end), change page to next */
                if (
                        (Settings::Instance()->getScrollChangesPage()) &&
                        (!m_timer_scrollPage->isActive())
                        )
                {
                    this->start_timerScrollPage();
                    emit pageNext();
                }
            }
            /* If page is at top, change page to previous */
            else if (
                     (Settings::Instance()->getScrollChangesPage()) &&
                     (event->delta() > 0) &&
                     (m_boundingRect.y() == 0) &&
                     (!m_timer_scrollPage->isActive())
                     )
            {
                this->start_timerScrollPage();
                m_flag_jumpToEnd = Settings::Instance()->getJumpToEnd();
                emit pagePrevious();
            }
            /* Scroll vertically; If page is scrolled to top or bottom, start timer */
            else
            {
                ScrollPageVertical(event->delta());

                if (
                        (
                            ((m_boundingRect.height() + m_boundingRect.y()) == this->size().height()) ||
                            (0 == m_boundingRect.y())
                            ) &&
                        (!m_timer_scrollPage->isActive())
                        )
                {
                    this->start_timerScrollPage();
                }
            }

        }
    }
    else if (
             (Qt::ControlModifier == event->modifiers()) ||
             (
                 (Qt::NoModifier == event->modifiers()) &&
                 (Wheel::Zoom == Settings::Instance()->getWheel())
                 )
             )
    {
        if (event->delta() < 0)
        {
            this->zoomOut();
            event->accept();
        }
        else
        {
            this->zoomIn();
            event->accept();
        }
    }
    else if ((Qt::ControlModifier | Qt::ShiftModifier) == event->modifiers())
    {
        this->setZoom(m_zoom_value * (1 + ((event->delta() / 4.8) / 100))); /* For standard scroll (+-120), zoom +-25% */
        event->accept();
    }
    else if (Qt::ShiftModifier == event->modifiers())
    {
        this->ScrollPageVertical(event->delta());
        event->accept();
    }
    else if (Qt::AltModifier == event->modifiers())
    {
        this->ScrollPageHorizontal(event->delta());
        event->accept();
    }
}


void PictureItem::start_timerScrollPage()
{
    if (Settings::Instance()->getPageChangeTimeout() > 0)
    {
        m_timer_scrollPage->start(Settings::Instance()->getPageChangeTimeout());
    }
}

/* End Region Rotation */



/* Region Zoom */



QPoint PictureItem::pointToOrigin(int width, int height)
{
    qreal zoomX = (qreal)width / (qreal)m_boundingRect.width();
    qreal zoomY = (qreal)height / (qreal)m_boundingRect.height();

    if (width > this->size().width())
    {
        qreal oldX = (m_boundingRect.x() - (m_boundingRect.x() * 2)) + (this->size().width() / 2);
        qreal oldY = (m_boundingRect.y() - (m_boundingRect.y() * 2)) + (this->size().height() / 2);

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
            qreal oldY = (m_boundingRect.y() - (m_boundingRect.y() * 2)) + (this->size().height() / 2);

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
    for (int i = 0; i < m_defaultZoomSizes.size(); ++i)
    {
        if (m_defaultZoomSizes.at(i) > m_zoom_value)
        {
            this->setZoom(m_defaultZoomSizes.at(i));
            return;
        }
    }

    this->setZoom(m_zoom_value * 1.25);
}

void PictureItem::zoomOut()
{
    for (int i = 0; i < m_defaultZoomSizes.size(); ++i)
    {
        if (m_defaultZoomSizes.at(i) >= m_zoom_value)
        {
            if (i != 0)
            {
                this->setZoom(m_defaultZoomSizes.at(i - 1));
            }
            else
            {
                this->setZoom(m_zoom_value / 1.25);
            }
            return;
        }
    }

    setZoom(m_zoom_value / 1.25);
}

void PictureItem::fitToScreen()
{
    if (this->isPixmapNull())
    {
        return;
    }

    QRect temp = QRect(m_boundingRect.x(), m_boundingRect.y(), m_boundingRect.width() / m_zoom_value, m_boundingRect.height() / m_zoom_value);

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

    qreal tw = m_boundingRect.width() / m_zoom_value;

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

    qreal th = m_boundingRect.height() / m_zoom_value;

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

/* Use in setpixmap and resize events */
void PictureItem::updateLockMode()
{
    if (this->isPixmapNull())
    {
        return;
    }

    switch (m_lockMode)
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

    /* Am I lined out to the left? */
    if (m_boundingRect.x() >= 0)
    {
        m_boundingRect.moveLeft(0);
    }
    else if ((m_boundingRect.x() <= (m_boundingRect.width() - this->size().width()) - ((m_boundingRect.width() - this->size().width()) * 2)))
    {
        if ((m_boundingRect.width() - this->size().width()) - ((m_boundingRect.width() - this->size().width()) * 2) <= 0)
        {
            /* I am too far to the left! */
            m_boundingRect.moveLeft((m_boundingRect.width() - this->size().width()) - ((m_boundingRect.width() - this->size().width()) * 2));
        }
        else
        {
            /* I am too far to the right! */
            m_boundingRect.moveLeft(0);
        }
    }

    /* Am I lined out to the top? */
    if (m_boundingRect.y() >= 0)
    {
        m_boundingRect.moveTop(0);
    }
    else if ((m_boundingRect.y() <= (m_boundingRect.height() - this->size().height()) - ((m_boundingRect.height() - this->size().height()) * 2)))
    {
        if ((m_boundingRect.height() - this->size().height()) - ((m_boundingRect.height() - this->size().height()) * 2) <= 0)
        {
            /* I am too far to the top! */
            m_boundingRect.moveTop((m_boundingRect.height() - this->size().height()) - ((m_boundingRect.height() - this->size().height()) * 2));
        }
        else
        {
            /* I am too far to the bottom! */
            m_boundingRect.moveTop(0);
        }
    }
}

/* End Region Zoom */



/* Region Drag */

void PictureItem::drag(const QPoint &pt)
{
    if (this->isPixmapNull())
    {
        return;
    }

    if (m_dragging)
    {
        /* Am I dragging it outside of the panel? */
        if ((pt.x() - m_point_drag.x() >= (m_boundingRect.width() - this->size().width()) - ((m_boundingRect.width() - this->size().width()) * 2)) && (pt.x() - m_point_drag.x() <= 0))
        {
            /* No, everything is just fine */
            m_boundingRect.moveLeft(pt.x() - m_point_drag.x());
        }
        else if ((pt.x() - m_point_drag.x() > 0))
        {
            /* Now don't drag it out of the panel please */
            m_boundingRect.moveLeft(0);
        }
        else if ((pt.x() - m_point_drag.x() < (m_boundingRect.width() - this->size().width()) - ((m_boundingRect.width() - this->size().width()) * 2)))
        {
            /* I am dragging it out of my panel. How many pixels do I have left? */
            if ((m_boundingRect.width() - this->size().width()) - ((m_boundingRect.width() - this->size().width()) * 2) <= 0)
            {
                /* Make it fit perfectly */
                m_boundingRect.moveLeft((m_boundingRect.width() - this->size().width()) - ((m_boundingRect.width() - this->size().width()) * 2));
            }
        }

        /* Am I dragging it outside of the panel? */
        if (pt.y() - m_point_drag.y() >= (m_boundingRect.height() - this->size().height()) - ((m_boundingRect.height() - this->size().height()) * 2) && (pt.y() - m_point_drag.y() <= 0))
        {
            /* No, everything is just fine */
            m_boundingRect.moveTop(pt.y() - m_point_drag.y());
        }
        else if ((pt.y() - m_point_drag.y() > 0))
        {
            /* Now don't drag it out of the panel please */
            m_boundingRect.moveTop(0);
        }
        else if (pt.y() - m_point_drag.y() < (m_boundingRect.height() - this->size().height()) - ((m_boundingRect.height() - this->size().height()) * 2))
        {
            /* I am dragging it out of my panel. How many pixels do I have left? */
            if ((m_boundingRect.height() - this->size().height()) - ((m_boundingRect.height() - this->size().height()) * 2) <= 0)
            {
                /* Make it fit perfectly */
                m_boundingRect.moveTop((m_boundingRect.height() - this->size().height()) - ((m_boundingRect.height()- this->size().height()) * 2));
            }
        }

        if (m_opengl)
        {
            m_imageDisplay_gl->update();
        }
        else
        {
            m_imageDisplay_raster->update();
        }
    }
}

void PictureItem::beginDrag(const QPoint &pt)
{
    if (this->isPixmapNull())
    {
        return;
    }

    /* Initial drag position */
    m_point_drag.setX(pt.x() - m_boundingRect.x());
    m_point_drag.setY(pt.y() - m_boundingRect.y());
    m_dragging = true;
    this->setCursor(Qt::ClosedHandCursor);
}

void PictureItem::endDrag()
{
    if (this->isPixmapNull())
    {
        return;
    }

    m_dragging = false;
    this->setCursor(Qt::OpenHandCursor);
}

/* End Region Drag */

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
