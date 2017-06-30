#include "widgets/picture_item/pictureitem.h"

#include <QMouseEvent>
#include <QImageReader>

PictureItem::PictureItem(
        const DataLoader * const data_loader,
        const IPictureLoader * const picture_loader,
        const Settings * const settings,
        QWidget *parent,
        Qt::WindowFlags f)
    : QWidget(parent, f)

    , m_data_loader(data_loader)
    , m_picture_loader(picture_loader)
    , m_movie(std::unique_ptr<QMovie>(new QMovie()))
    , m_data(new PictureItemData())

    , m_settings(settings)
    , m_imageDisplay(nullptr)
    , m_watcher_data(new QFutureWatcher<QByteArray>(this))
    , m_watcher_image(new QFutureWatcher<QImage>(this))

    , m_lockMode(LockMode::None)
    , m_dragging(false)
    , m_is_animation_first_frame(true)
{
    this->setCursor(Qt::OpenHandCursor);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_watcher_data, &QFutureWatcher<QByteArray>::resultReadyAt,
            this, &PictureItem::dataLoaded);
    connect(m_watcher_image, &QFutureWatcher<QImage>::resultReadyAt,
            this, &PictureItem::imageFinished);

    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setSpacing(0);
    layoutMain->setMargin(0);
    this->setLayout(layoutMain);

    initPictureItem(m_settings->getHardwareAcceleration());
}

PictureItem::~PictureItem()
{
    delete m_data;
    delete m_imageDisplay;
}

void PictureItem::initPictureItem(bool opengl)
{
    if (m_imageDisplay != nullptr)
    {
        this->layout()->removeWidget(m_imageDisplay->getWidget());
//        m_imageDisplay->getWidget()->deleteLater();
        delete m_imageDisplay;
    }

    if (opengl)
    {
        m_imageDisplay = new PictureItemGL(m_data, m_settings->getZoomFilter(), this);
    }
    else
    {
        m_imageDisplay = new PictureItemRaster(m_data, m_settings->getZoomFilter(), this);
    }
    this->layout()->addWidget(m_imageDisplay->getWidget());
}


/* Region Image loading */

void PictureItem::setPixmap(const FileInfo &info)
{
#ifdef DEBUG_PICTUREITEM
    DEBUGOUT << info.getPath();
#endif
    m_movie->stop();
    m_movie->setDevice(nullptr);
    m_watcher_data->cancel();
    m_watcher_image->cancel();
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
        m_watcher_data->setFuture(
                    QtConcurrent::run(
                        m_data_loader, &DataLoader::getData, info, -1));
    }
}

void PictureItem::dataLoaded(int num)
{
    const QByteArray data = m_watcher_data->resultAt(num);
    if (data.isEmpty())
    {
        m_imageDisplay->getWidget()->setUpdatesEnabled(true);
        m_data->setPixmapNull(true);
        return;
    }
    auto f = [this, data]() {
        std::unique_ptr<QBuffer> buffer = std::unique_ptr<QBuffer>(new QBuffer());
        buffer->setData(data);
        QImageReader ir(buffer.get());
        if (ir.supportsAnimation())
        {
            m_animation_buffer = std::move(buffer);
            return QImage(ir.size(), ir.imageFormat());
        }
        else
        {
            m_animation_buffer = nullptr;
            return m_picture_loader->getImage(std::move(buffer));
        }
    };
    m_watcher_image->setFuture(QtConcurrent::run(f));
    /* Free result memory */
    m_watcher_data->setFuture(QFuture<QByteArray>());
}

void PictureItem::imageFinished(int num)
{
    const QImage newImage = m_watcher_image->resultAt(num);
#ifdef DEBUG_PICTUREITEM
    DEBUGOUT << t.restart() << newImage.size();
#endif
    m_imageDisplay->setBackgroundColor(getAverageColor(newImage));
    m_imageDisplay->setImage(newImage);

    this->afterImageLoad(newImage);
//    m_imageDisplay->getWidget()->setUpdatesEnabled(true);

    /* Free result memory */
    m_watcher_image->setFuture(QFuture<QImage>());
    if (m_animation_buffer != nullptr)
    {
        m_movie->setDevice(m_animation_buffer.get());
        QObject::connect(m_movie.get(), &QMovie::updated, [&](const QRect &/*rect*/) {
            const QImage img = m_movie->currentImage();
            m_imageDisplay->setImage(img);
            if (m_is_animation_first_frame)
            {
                m_is_animation_first_frame = false;
                this->afterImageLoad(img);
            }
        });
        m_is_animation_first_frame = true;
        m_movie->start();
    }
}

void PictureItem::afterImageLoad(const QImage& img)
{
    m_data->setPixmapNull(img.isNull());
    m_data->setWidgetSize(m_imageDisplay->getWidget()->size());
    m_data->setImageSize(img.size());
    if (m_lockMode == LockMode::None)
    {
        this->setZoom(1);
    }
    this->setRotation(0);

    this->updateLockMode();
    m_data->resetImagePosition(m_settings->getRightToLeft());
    m_data->updateOffsets();
    emit imageChanged();
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
        this->scrollImageVertical(120);
        event->accept();
        break;

    case Qt::Key_Down:
        this->scrollImageVertical(-120);
        event->accept();
        break;

    case Qt::Key_Left:
        this->scrollImageHorizontal(120);
        event->accept();
        break;

    case Qt::Key_Right:
        this->scrollImageHorizontal(-120);
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

void PictureItem::scrollBy(const int x, const int y) {
    m_data->beginDrag(QPoint(0, 0));
    m_data->drag(QPoint(x, y));
    m_imageDisplay->getWidget()->update();
}

void PictureItem::scrollImageVertical(const int value)
{
    scrollBy(0, value);
}

void PictureItem::scrollImageHorizontal(const int value)
{
    scrollBy(value, 0);
}

/* End Region Drag */
