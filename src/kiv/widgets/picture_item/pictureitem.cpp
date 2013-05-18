#include "widgets/picture_item/pictureitem.h"

#include <QMouseEvent>

PictureItem::PictureItem(const Settings * const settings, QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)

    , m_data(new PictureItemData())

    , m_settings(settings)
    , m_opengl(settings->getHardwareAcceleration())
    , m_imageDisplay(0)
    , m_loader_image(new QFutureWatcher<QImage>(this))
    , m_timer_scrollPage(new QTimer(this))

    , m_dragging(false)
    , m_point_drag(QPoint())
{
    this->setCursor(Qt::OpenHandCursor);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    m_timer_scrollPage->setSingleShot(true);

    connect(m_loader_image, SIGNAL(resultReadyAt(int)), this, SLOT(imageFinished(int)));

    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setSpacing(0);
    layoutMain->setMargin(0);
    this->setLayout(layoutMain);

    initPictureItem();
}

PictureItem::~PictureItem()
{
    delete m_data;
    delete m_imageDisplay;
}

void PictureItem::initPictureItem()
{
#ifndef QT5
    if (m_opengl)
    {
        m_imageDisplay = new PictureItemGL(m_data, this);
    }
    else
#endif
    {
        m_imageDisplay = new PictureItemRaster(m_data, this);
    }
    this->layout()->addWidget(m_imageDisplay->getWidget());
}

void PictureItem::setHardwareAcceleration(const bool b)
{
    if (m_opengl != b)
    {
#ifdef DEBUG_PICTUREITEM
        qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureItem::setHardwareAcceleration" << "Deleted m_imageDisplay";
#endif
        this->layout()->removeWidget(m_imageDisplay->getWidget());
//        m_imageDisplay->getWidget()->deleteLater();
        delete m_imageDisplay;

        m_opengl = b;
        initPictureItem();
    }
}



/* Region Image loading */

void PictureItem::setPixmap(const FileInfo &info)
{
#ifdef DEBUG_PICTUREITEM
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureItem::setPixmap" << info.getPath();
#endif
    m_imageDisplay->getWidget()->setUpdatesEnabled(false);
    m_imageDisplay->setNullImage();
    if (!info.fileExists())
    {
//        calculateAverageColor(QImage());

//        m_imageDisplay->setNullImage();

        m_imageDisplay->getWidget()->setUpdatesEnabled(true);
        m_data->setPixmapNull(true);
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
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureItem::imageFinished" << t.restart() << m_loader_image->resultAt(num).size();
#endif
    m_data->setPixmapNull(m_loader_image->resultAt(num).isNull());
    calculateAverageColor(m_loader_image->resultAt(num));

    m_imageDisplay->setImage(m_loader_image->resultAt(num));

    m_data->m_boundingRect = QRect(0, 0, m_loader_image->resultAt(num).width(), m_loader_image->resultAt(num).height());

    this->afterPixmapLoad();

    m_data->updateSize(m_imageDisplay->getWidget()->size());

//    m_imageDisplay->getWidget()->setUpdatesEnabled(true);
    emit imageChanged();

    /* Free result memory */
    m_loader_image->setFuture(QFuture<QImage>());
}

void PictureItem::afterPixmapLoad()
{
    if (m_data->getLockMode() == LockMode::None)
    {
        this->setZoom(1);
    }
    this->setRotation(0);

    this->updateLockMode();

    if (m_data->m_boundingRect.width() > m_imageDisplay->getWidget()->width())
    {
        if (m_settings->getRightToLeft() ^ m_data->m_flag_jumpToEnd)
        {
            m_data->m_boundingRect.moveLeft(-(m_data->m_boundingRect.width() - m_imageDisplay->getWidget()->width()));
        }
    }

    if (m_data->m_flag_jumpToEnd)
    {
        if (m_data->m_boundingRect.height() > m_imageDisplay->getWidget()->height())
        {
            m_data->m_boundingRect.moveTop(-(m_data->m_boundingRect.height() - m_imageDisplay->getWidget()->height()));
        }
        m_data->m_flag_jumpToEnd = false;
    }
    else
    {
        m_data->m_boundingRect.moveTop(0);
    }
}

void PictureItem::calculateAverageColor(const QImage &img)
{
    if (m_settings->getCalculateAverageColor() && !img.isNull())
    {
        const QImage averageColorImage = img.scaled(1,1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_imageDisplay->setBackgroundColor(QColor::fromRgb(averageColorImage.pixel(0,0)));
    }
    else
    {
        if (m_data->m_color_clear != Qt::lightGray)
        {
            m_imageDisplay->setBackgroundColor(Qt::lightGray);
        }
    }
}

/* End Region Image loading */



void PictureItem::mousePressEvent(QMouseEvent *event)
{
    this->setFocus();

    if (event->button() == Qt::LeftButton)
    {
        /* Start dragging */
        this->beginDrag(event->pos());
    }
    else
    {
        emit mousePress(event);
    }

    return QWidget::mousePressEvent(event);
}


/* End Region Drag */


void PictureItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit mouseDoubleClick(event);
    return QWidget::mouseDoubleClickEvent(event);
}

void PictureItem::resizeEvent(QResizeEvent *event)
{
    if (this->isPixmapNull())
    {
        return;
    }

    m_data->updateSize(m_imageDisplay->getWidget()->size());

    m_data->avoidOutOfScreen(m_imageDisplay->getWidget()->size());
    this->updateLockMode();

    return QWidget::resizeEvent(event);
}



void PictureItem::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Up:
        this->ScrollPageVertical(120);
        event->accept();
        break;

    case Qt::Key_Down:
        this->ScrollPageVertical(-120);
        event->accept();
        break;

    case Qt::Key_Left:
        this->ScrollPageHorizontal(120);
        event->accept();
        break;

    case Qt::Key_Right:
        this->ScrollPageHorizontal(-120);
        event->accept();
        break;
    }
    return QWidget::keyPressEvent(event);
}

void PictureItem::wheelEvent(QWheelEvent *event)
{
    /* event->delta() > 0 == Up
       event->delta() < 0 == Down */
    if (
            (Qt::ControlModifier == event->modifiers()) ||
            (
                (Qt::NoModifier == event->modifiers()) &&
                (WheelAction::Zoom == m_settings->getWheel())
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
    else if (Qt::NoModifier == event->modifiers())
    {
        /* If page can't be scrolled, change page if necessary */
        if (WheelAction::ChangePage == m_settings->getWheel() ||
                (
                    (WheelAction::Scroll == m_settings->getWheel()) &&
                    (m_settings->getScrollChangesPage()) &&
                    (
                        (LockMode::Autofit == m_data->getLockMode()) ||
                        (m_data->m_boundingRect.height() <= m_imageDisplay->getWidget()->height() && m_data->m_boundingRect.width() <= m_imageDisplay->getWidget()->width())
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
                if (WheelAction::Scroll == m_settings->getWheel())
                {
                    m_data->m_flag_jumpToEnd = m_settings->getJumpToEnd();
                }
                emit pagePrevious();
                event->accept();
            }
        }
        /* Scroll page */
        else if (WheelAction::Scroll == m_settings->getWheel())
        {
            event->accept();
            if (
                    (event->delta() < 0) &&
                    (!m_timer_scrollPage->isActive()) &&
                    ((-m_data->m_boundingRect.y() + m_imageDisplay->getWidget()->height()) >= m_data->m_boundingRect.height())
                    )
            {
                /* Scroll horizontally; If page is scrolled to end, start timer */
                if (m_settings->getScrollPageByWidth())
                {
                    if (m_settings->getRightToLeft())
                    {
                        if (m_data->m_boundingRect.x() < 0)
                        {
                            this->ScrollPageHorizontal(-event->delta());

                            if (0 == m_data->m_boundingRect.x())
                            {
                                this->start_timerScrollPage();
                            }
                            return;
                        }
                    }
                    else
                    {
                        if ((m_data->m_boundingRect.width() + m_data->m_boundingRect.x()) > m_imageDisplay->getWidget()->width())
                        {
                            this->ScrollPageHorizontal(event->delta());


                            if ((m_data->m_boundingRect.width() + m_data->m_boundingRect.x()) == m_imageDisplay->getWidget()->width())
                            {
                                this->start_timerScrollPage();
                            }
                            return;
                        }
                    }
                }
                /* If this code is reached (page was already scrolled to end), change page to next */
                if (
                        (m_settings->getScrollChangesPage()) &&
                        (!m_timer_scrollPage->isActive())
                        )
                {
                    this->start_timerScrollPage();
                    emit pageNext();
                }
            }
            /* If page is at top, change page to previous */
            else if (
                     (m_settings->getScrollChangesPage()) &&
                     (event->delta() > 0) &&
                     (0 == m_data->m_boundingRect.y()) &&
                     (!m_timer_scrollPage->isActive())
                     )
            {
                this->start_timerScrollPage();
                m_data->m_flag_jumpToEnd = m_settings->getJumpToEnd();
                emit pagePrevious();
            }
            /* Scroll vertically; If page is scrolled to top or bottom, start timer */
            else
            {
                ScrollPageVertical(event->delta());

                if (
                        (
                            ((m_data->m_boundingRect.height() + m_data->m_boundingRect.y()) == m_imageDisplay->getWidget()->height()) ||
                            (0 == m_data->m_boundingRect.y())
                            ) &&
                        (!m_timer_scrollPage->isActive())
                        )
                {
                    this->start_timerScrollPage();
                }
            }

        }
    }
    else if ((Qt::ControlModifier | Qt::ShiftModifier) == event->modifiers())
    {
        this->setZoom(m_data->getZoom() * (1 + ((event->delta() / 4.8) / 100))); /* For standard scroll (+-120), zoom +-25% */
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

    return QWidget::wheelEvent(event);
}


void PictureItem::start_timerScrollPage()
{
    if (m_settings->getPageChangeTimeout() > 0)
    {
        m_timer_scrollPage->start(m_settings->getPageChangeTimeout());
    }
}


/* Region Rotation */

void PictureItem::setRotation(const qreal value)
{
    if (isPixmapNull())
    {
        return;
    }

    const qreal previous = m_data->getRotation();
    m_data->setRotation(value);

    m_imageDisplay->setRotation(m_data->getRotation(), previous);
}

/* End Region Rotation */


/* Region Zoom */

void PictureItem::setZoom(const qreal value)
{
    if (m_data->isPixmapNull())
    {
        return;
    }

    const qreal previous = m_data->getZoom();
    m_data->setZoom(value);
    m_imageDisplay->setZoom(m_data->getZoom(), previous);

    emit zoomChanged(m_data->getZoom(), previous);
}


void PictureItem::fitToScreen()
{
    if (this->isPixmapNull())
    {
        return;
    }

    const QRect orig_size = QRect(m_data->m_boundingRect.x(),
                                  m_data->m_boundingRect.y(),
                                  m_data->m_boundingRect.width() / m_data->getZoom(),
                                  m_data->m_boundingRect.height() / m_data->getZoom());

    const qreal x_ratio = (qreal)m_imageDisplay->getWidget()->width() / orig_size.width();
    const qreal y_ratio = (qreal)m_imageDisplay->getWidget()->height() / orig_size.height();

    if ((orig_size.width() <= m_imageDisplay->getWidget()->width()) && (orig_size.height() <= m_imageDisplay->getWidget()->height()))
    {
        this->setZoom(1);
    }
    else if ((x_ratio * orig_size.height()) < m_imageDisplay->getWidget()->height())
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

    const qreal tw = m_data->m_boundingRect.width() / m_data->getZoom();

    const qreal x_ratio = (qreal)m_imageDisplay->getWidget()->width() / tw;

    if (tw <= m_imageDisplay->getWidget()->width())
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

    const qreal th = m_data->m_boundingRect.height() / m_data->getZoom();

    const qreal y_ratio = (qreal)m_imageDisplay->getWidget()->height() / th;

    if (th <= m_imageDisplay->getWidget()->height())
    {
        this->setZoom(1);
    }
    else
    {
        this->setZoom(y_ratio);
    }

}

void PictureItem::setLockMode(const int mode)
{
    m_data->setLockMode(mode);
    this->updateLockMode();
}

/* Use in setpixmap and resize events */
void PictureItem::updateLockMode()
{
    if (this->isPixmapNull())
    {
        return;
    }

    switch (m_data->getLockMode())
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

/* End Region Zoom */



/* Region Drag */

void PictureItem::mouseMoveEvent(QMouseEvent *event)
{
    this->drag(event->pos());

    return QWidget::mouseMoveEvent(event);
}

void PictureItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_dragging && (event->button() == Qt::LeftButton))
    {
        this->endDrag();
    }

    return QWidget::mouseReleaseEvent(event);
}

void PictureItem::drag(const QPoint &pt)
{
    if (this->isPixmapNull())
    {
        return;
    }

    if (m_dragging)
    {
        const QSize widgetSize = m_imageDisplay->getWidget()->size();
        const qreal widthDiff = widgetSize.width() - m_data->m_boundingRect.width();
        const qreal heightDiff = widgetSize.height() - m_data->m_boundingRect.height();
        const int xDiff = pt.x() - m_point_drag.x();
        const int yDiff = pt.y() - m_point_drag.y();

        /* Am I dragging it outside of the panel? */
        if ((xDiff >= widthDiff) && (xDiff <= 0))
        {
            /* No, everything is just fine */
            m_data->m_boundingRect.moveLeft(xDiff);
        }
        else if (xDiff > 0)
        {
            /* Now don't drag it out of the panel please */
            m_data->m_boundingRect.moveLeft(0);
        }
        else if (xDiff < widthDiff)
        {
            /* I am dragging it out of my panel. How many pixels do I have left? */
            if (widthDiff <= 0)
            {
                /* Make it fit perfectly */
                m_data->m_boundingRect.moveLeft(widthDiff);
            }
        }

        /* Am I dragging it outside of the panel? */
        if (yDiff >= heightDiff && (yDiff <= 0))
        {
            /* No, everything is just fine */
            m_data->m_boundingRect.moveTop(yDiff);
        }
        else if (yDiff > 0)
        {
            /* Now don't drag it out of the panel please */
            m_data->m_boundingRect.moveTop(0);
        }
        else if (yDiff < heightDiff)
        {
            /* I am dragging it out of my panel. How many pixels do I have left? */
            if (heightDiff <= 0)
            {
                /* Make it fit perfectly */
                m_data->m_boundingRect.moveTop(heightDiff);
            }
        }

        m_imageDisplay->getWidget()->update();
    }
}

void PictureItem::beginDrag(const QPoint &pt)
{
    if (this->isPixmapNull())
    {
        return;
    }

    /* Initial drag position */
    m_point_drag.setX(pt.x() - m_data->m_boundingRect.x());
    m_point_drag.setY(pt.y() - m_data->m_boundingRect.y());
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

void PictureItem::ScrollPageVertical(const int value)
{
    this->beginDrag(QPoint(0,0));
    this->drag(QPoint(0,value));
    this->endDrag();
}

void PictureItem::ScrollPageHorizontal(const int value)
{
    this->beginDrag(QPoint(0,0));
    this->drag(QPoint(value,0));
    this->endDrag();
}

/* End Region Drag */
