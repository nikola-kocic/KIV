#include "widgets/picture_item/pictureitem_gl.h"

#include "settings.h"
#include "picture_loader.h"

//#define DEBUG_PICTUREITEM_GL
#ifdef DEBUG_PICTUREITEM_GL
#include "helper.h"
#endif

QImage CreatePow2Bitmap(const TexIndex *const index);

PictureItemGL::PictureItemGL(PictureItemData *data, ZoomFilter zoomFilter, QWidget *parent)
    : QGLWidget(parent)
    , PictureItemInterface(data)
    , m_loader_texture(new QFutureWatcher<QImage>(this))
    , m_scaleX(0)
    , m_scaleY(0)
    , m_textures(QVector < QVector < QOpenGLTexture* > >(0))
    , m_texImg(new TexImg())
    , m_returnTexCount(0)
    , m_zoomFilter(zoomFilter)
{
    m_widget = this;
    QGLContext::setTextureCacheLimit(0);
    connect(m_loader_texture, SIGNAL(resultReadyAt(int)),
            this, SLOT(textureFinished(int)));
}

PictureItemGL::~PictureItemGL()
{
#ifdef DEBUG_PICTUREITEM_GL
    DEBUGOUT;
#endif
    clearTextures();
    delete m_texImg;
}

void PictureItemGL::clearTextures()
{
    /* Delete old textures */
    for (int hIndex = 0; hIndex < m_textures.size(); ++hIndex)
    {
        for (int vIndex = 0; vIndex < m_textures.at(hIndex).size(); ++vIndex)
        {
            QOpenGLTexture* tex = m_textures.at(hIndex).at(vIndex);
            if (tex != nullptr)
            {
                m_textures.at(hIndex).at(vIndex)->destroy();
            }
#ifdef DEBUG_PICTUREITEM_GL
            DEBUGOUT << "deleted texture"
                     << this->m_textures.at(hIndex).at(vIndex)
                     << "@" << hIndex << vIndex;
#endif
        }
    }

    m_textures = QVector < QVector < QOpenGLTexture* > >(0);
}

void PictureItemGL::setImage(const QImage &img)
{
    m_loader_texture->cancel();
    this->setUpdatesEnabled(false);

    m_texImg->setImage(img);

    m_textures = QVector < QVector < QOpenGLTexture* > >(m_texImg->hTile->tileCount);
    QList<TexIndex *> indexes;
    for (int hIndex = 0; hIndex < m_texImg->hTile->tileCount; ++hIndex)
    {
        m_textures[hIndex].resize(m_texImg->vTile->tileCount);

        for (int vIndex = 0; vIndex < m_texImg->vTile->tileCount; ++vIndex)
        {
            TexIndex *tex = new TexIndex();
            tex->bitmapData = img;
            tex->background = m_data->m_color_clear;
            tex->currentTileWidth = m_texImg->hTile->tileSize.at(hIndex);
            tex->currentTileHeight = m_texImg->vTile->tileSize.at(vIndex);
            tex->hBorderOffset = m_texImg->hTile->offsetBorder.at(hIndex);
            tex->vBorderOffset = m_texImg->vTile->offsetBorder.at(vIndex);
            indexes.append(tex);
        }
    }

    this->loadTextures(indexes);
}

void PictureItemGL::setNullImage()
{
    m_loader_texture->cancel();
    m_returnTexCount = 0;
    clearTextures();
    m_texImg->setImage(QImage());

    this->updateGL();
}

QImage CreatePow2Bitmap(const TexIndex *const index)
{
    const QImage i = TexImg::CreatePow2Bitmap(index);
    delete index;
    return i;
}

void PictureItemGL::loadTextures(QList<TexIndex *> indexes)
{
    m_returnTexCount = indexes.size();
    m_loader_texture->setFuture(QtConcurrent::mapped(indexes,
                                                     CreatePow2Bitmap));
}

void PictureItemGL::textureFinished(int num)
{
    this->setTexture(m_loader_texture->resultAt(num), num);
    if (--m_returnTexCount == 0)
    {
        m_data->setPixmapNull(false);
        m_loader_texture->setFuture(QFuture<QImage>());

        /* Update view */

        this->setUpdatesEnabled(true);
        this->updateGL();

#ifdef DEBUG_PICTUREITEM
        DEBUGOUT << "loaded textures" << t.elapsed();
#endif
    }
}

void PictureItemGL::setTexture(const QImage &tex, const int num)
{
    QOpenGLTexture* gltex = new QOpenGLTexture(tex);
    const int hIndex = num / m_texImg->vTile->tileCount;
    const int vIndex = num % m_texImg->vTile->tileCount;

    switch(m_zoomFilter)
    {
    case ZoomFilter::None:
        gltex->setMinificationFilter(QOpenGLTexture::Nearest);
        gltex->setMagnificationFilter(QOpenGLTexture::Nearest);
        break;
    case ZoomFilter::Good:
        gltex->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        gltex->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
        break;
    }
    m_textures[hIndex][vIndex] = gltex;

#ifdef DEBUG_PICTUREITEM_GL
    DEBUGOUT << "bound texture" << this->m_textures.at(hIndex).at(vIndex)
             << "@" << hIndex << vIndex <<";" << tex.size();
#endif
}


void PictureItemGL::setBackgroundColor(const QColor &color)
{
    PictureItemInterface::setBackgroundColor(color);
    qglClearColor(color);
//    this->updateGL();
}

void PictureItemGL::initializeGL()
{
    initializeOpenGLFunctions();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    qglClearColor(m_data->m_color_clear);

    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_PaintOnScreen, true);

    GLint size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);

#ifdef DEBUG_PICTUREITEM_GL
    DEBUGOUT << "GL_MAX_TEXTURE_SIZE" << size;
#endif

    m_texImg->setTexMaxSize(size);
}

void PictureItemGL::updateSize()
{
    if (m_textures.size() == 0)
    {
        return;
    }

    const QSizeF img_size_original = m_data->getImageSizeOriginal();
    m_scaleX = (img_size_original.width() * m_data->getZoom()) / this->width();
    m_scaleY = (img_size_original.height() * m_data->getZoom()) / this->height();
}

void PictureItemGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_textures.size() == 0)
    {
        return;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const QPointF offset = m_data->getOffset();
    const QSizeF img_size_original = m_data->getImageSizeOriginal();
    const QRectF bounding_rect = m_data->getBoundingRect();
    glTranslated((bounding_rect.x() + offset.x() + bounding_rect.width() / 2),
                 (bounding_rect.y() + offset.y() + bounding_rect.height() / 2),
                 0);

    glRotated(m_data->getRotation(), 0, 0, 1);
    glTranslated(-(img_size_original.width() * m_data->getZoom()) / 2,
                 -(img_size_original.height() * m_data->getZoom()) / 2,
                 0);
    glScaled(m_scaleX, m_scaleY, 1);

    const QRectF texImage = QRectF(QPointF(0.0, 0.0), QPointF (1.0, 1.0));
    const QRectF vertImage = QRectF(QPointF(0, 0),
                                    QPointF (this->width(), this->height()));

    for (int hIndex = 0; hIndex < m_texImg->hTile->tileCount; ++hIndex)
    {
        double texScale = 1.0 * img_size_original.width() / m_texImg->hTile->tileSize.at(hIndex);
        double tx;
        double tx2;
        double qx;
        double qx2;

        if (!ClipTextureVertex(
                texImage.left(),
                texImage.right(),
                vertImage.left(),
                vertImage.right(),
                m_texImg->hTile->switchBorderNorm.at(hIndex),
                m_texImg->hTile->switchBorderNorm.at(hIndex + 1),
                m_texImg->hTile->offsetBorderNorm.at(hIndex),
                texScale,
                tx,
                tx2,
                qx,
                qx2))
        {
            continue;
        }

        for (int vIndex = 0; vIndex < m_texImg->vTile->tileCount; ++vIndex)
        {
            texScale = 1.0 * img_size_original.height() / m_texImg->vTile->tileSize.at(vIndex);
            double ty;
            double ty2;
            double qy;
            double qy2;

            if (!ClipTextureVertex(
                    texImage.top(),
                    texImage.bottom(),
                    vertImage.top(),
                    vertImage.bottom(),
                    m_texImg->vTile->switchBorderNorm.at(vIndex),
                    m_texImg->vTile->switchBorderNorm.at(vIndex + 1),
                    m_texImg->vTile->offsetBorderNorm.at(vIndex),
                    texScale,
                    ty,
                    ty2,
                    qy,
                    qy2))
            {
                continue;
            }

            m_textures.at(hIndex).at(vIndex)->bind();
            glBegin(GL_QUADS);

            glTexCoord2d(tx, ty);
            glVertex2d(qx, qy);
            glTexCoord2d(tx, ty2);
            glVertex2d(qx, qy2);
            glTexCoord2d(tx2, ty2);
            glVertex2d(qx2, qy2);
            glTexCoord2d(tx2, ty);
            glVertex2d(qx2, qy);
            glEnd();
        }
    }
}

void PictureItemGL::resizeGL(int width, int height)
{
    this->updateSize();
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void PictureItemGL::setRotation(const qreal, const qreal)
{
    this->updateSize();
    this->updateGL();
}

void PictureItemGL::setZoom(const qreal, const qreal)
{
    this->updateSize();
    this->updateGL();
}
