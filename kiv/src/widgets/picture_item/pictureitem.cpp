#include "kiv/src/widgets/picture_item/pictureitem.h"

#include <QMouseEvent>

PictureItem::PictureItem(const IPictureLoader * const picture_loader,
                         const Settings * const settings,
                         QWidget *parent,
                         Qt::WindowFlags f)
    : QWidget(parent, f)

    , m_picture_loader(picture_loader)
    , m_data(new PictureItemData())

    , m_settings(settings)
    , m_opengl(settings->getHardwareAcceleration())
    , m_imageDisplay(nullptr)
    , m_loader_image(new QFutureWatcher<QImage>(this))

    , m_lockMode(LockMode::None)
    , m_dragging(false)
{
    this->setCursor(Qt::OpenHandCursor);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_loader_image, SIGNAL(resultReadyAt(int)),
            this, SLOT(imageFinished(int)));

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
    if (m_opengl)
    {
        m_imageDisplay = new PictureItemGL(m_data, this);
    }
    else
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
        DEBUGOUT << "Deleted m_imageDisplay";
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
    DEBUGOUT << info.getPath();
#endif
    m_loader_image->cancel();
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
        m_loader_image->setFuture(
                    QtConcurrent::run(
                        m_picture_loader, &IPictureLoader::getImage, info));
    }
}

void PictureItem::imageFinished(int num)
{
    const QImage newImage = m_loader_image->resultAt(num);
#ifdef DEBUG_PICTUREITEM
    DEBUGOUT << t.restart() << newImage.size();
#endif
    m_data->setPixmapNull(newImage.isNull());
    m_imageDisplay->setBackgroundColor(getAverageColor(newImage));

    m_imageDisplay->setImage(newImage);
    m_data->setWidgetSize(m_imageDisplay->getWidget()->size());
    m_data->setImageSize(newImage.size());

    this->afterPixmapLoad();

    m_data->updateOffsets();

//    m_imageDisplay->getWidget()->setUpdatesEnabled(true);
    emit imageChanged();

    /* Free result memory */
    m_loader_image->setFuture(QFuture<QImage>());
}

void PictureItem::afterPixmapLoad()
{
    if (m_lockMode == LockMode::None)
    {
        this->setZoom(1);
    }
    this->setRotation(0);

    this->updateLockMode();
    m_data->resetImagePosition(m_settings->getRightToLeft());
}

QColor PictureItem::getAverageColor(const QImage &img) const
{
    QColor retColor = Qt::lightGray;
    if (m_settings->getCalculateAverageColor() && !img.isNull())
    {
        const QImage averageColorImage = img.scaled(1, 1, Qt::IgnoreAspectRatio,
                                                    Qt::SmoothTransformation);
        retColor = QColor::fromRgb(averageColorImage.pixel(0,0));
    }
    return retColor;
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

    return QWidget::mousePressEvent(event);
}


/* End Region Drag */


void PictureItem::resizeEvent(QResizeEvent *event)
{
    if (this->isPixmapNull())
    {
        return;
    }

    m_data->setWidgetSize(m_imageDisplay->getWidget()->size());
    m_data->updateOffsets();
    m_data->avoidOutOfScreen();
    this->updateLockMode();

    return QWidget::resizeEvent(event);
}


void PictureItem::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Up:
        this->scrollPageVertical(120);
        event->accept();
        break;

    case Qt::Key_Down:
        this->scrollPageVertical(-120);
        event->accept();
        break;

    case Qt::Key_Left:
        this->scrollPageHorizontal(120);
        event->accept();
        break;

    case Qt::Key_Right:
        this->scrollPageHorizontal(-120);
        event->accept();
        break;
    default:
        return QWidget::keyPressEvent(event);
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
    qreal zoomVal;
    if (m_data->setZoom(value, zoomVal) != 0)
    {
        return;
    }
    m_imageDisplay->setZoom(zoomVal, previous);

    emit zoomChanged(zoomVal, previous);
}


void PictureItem::fitToScreen()
{
    qreal zoomVal;
    if (m_data->fitToScreen(zoomVal) != 0)
    {
        return;
    }
    this->setZoom(zoomVal);
}

void PictureItem::fitWidth()
{
    qreal zoomVal;
    if (m_data->fitWidth(zoomVal) != 0)
    {
        return;
    }
    this->setZoom(zoomVal);
}

void PictureItem::fitHeight()
{
    qreal zoomVal;
    if (m_data->fitHeight(zoomVal) != 0)
    {
        return;
    }
    this->setZoom(zoomVal);
}

void PictureItem::setLockMode(const LockMode mode)
{
    m_lockMode = mode;
    this->updateLockMode();
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
    if (this->isPixmapNull() || !m_dragging)
    {
        return;
    }

    m_data->drag(pt);
    m_imageDisplay->getWidget()->update();
}

void PictureItem::beginDrag(const QPoint &pt)
{
    if (this->isPixmapNull())
    {
        return;
    }

    /* Initial drag position */
    m_data->beginDrag(pt);
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

void PictureItem::scrollPageVertical(const int value)
{
    this->beginDrag(QPoint(0,0));
    this->drag(QPoint(0,value));
    this->endDrag();
}

void PictureItem::scrollPageHorizontal(const int value)
{
    this->beginDrag(QPoint(0,0));
    this->drag(QPoint(value,0));
    this->endDrag();
}

/* End Region Drag */
