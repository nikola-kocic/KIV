#ifndef QT5
#include "widgets/picture_item/pictureitem_gl.h"

//#define DEBUG_PICTUREITEM_GL

#ifdef DEBUG_PICTUREITEM_GL
#include <QDebug>
#include <QDateTime>
#endif

#include "settings.h"
#include "picture_loader.h"

PictureItemGL::PictureItemGL(PictureItemData *data, QWidget *parent)    
    : QGLWidget(parent)
    , PictureItemInterface(data, parent)
    , m_returnTexCount(0)
    , m_loader_texture(new QFutureWatcher<QImage>(this))
    , m_scaleX(0)
    , m_scaleY(0)
    , m_textures(QVector < QVector < GLuint > >(0))
    , m_texImg(new TexImg())
{
    m_widget = this;
    QGLContext::setTextureCacheLimit(0);
    connect(m_loader_texture, SIGNAL(resultReadyAt(int)), this, SLOT(textureFinished(int)));
}

PictureItemGL::~PictureItemGL()
{
#ifdef DEBUG_PICTUREITEM_GL
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "~PictureItemGL()";
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
            deleteTexture(m_textures.at(hIndex).at(vIndex));
#ifdef DEBUG_PICTUREITEM_GL
            qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureItemGL::clearTextures" <<
                        "deleted texture" << this->m_textures.at(hIndex).at(vIndex) << "@" << hIndex << vIndex;
#endif
        }
    }

    m_textures = QVector < QVector < GLuint > >(0);
}

void PictureItemGL::setImage(const QImage &img)
{
    this->setUpdatesEnabled(false);

    m_texImg->setImage(img);

    m_textures = QVector < QVector < GLuint > >(m_texImg->hTile->tileCount);
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
    m_returnTexCount = 0;
    clearTextures();
    m_texImg->setImage(QImage());

    this->updateGL();
}

void PictureItemGL::loadTextures(QList<TexIndex *> indexes)
{
    m_returnTexCount = indexes.size();
    m_loader_texture->setFuture(QtConcurrent::mapped(indexes, TexImg::CreatePow2Bitmap));
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
        qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureItemGL::textureFinished"
                 << "loaded textures" << t.elapsed();
#endif
    }
}

void PictureItemGL::setTexture(const QImage &tex, const int num)
{
    const int hIndex = num / m_texImg->vTile->tileCount;
    const int vIndex = num % m_texImg->vTile->tileCount;

    m_textures[hIndex][vIndex] = bindTexture(tex, GL_TEXTURE_2D, GL_RGB, QGLContext::LinearFilteringBindOption | QGLContext::MipmapBindOption);

#ifdef DEBUG_PICTUREITEM_GL
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureItemGL::setTexture"
             << "bound texture" << this->m_textures.at(hIndex).at(vIndex) << "@" << hIndex << vIndex <<";" << tex.size();
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
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    qglClearColor(m_data->m_color_clear);


    QGL::setPreferredPaintEngine(QPaintEngine::OpenGL2);

    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_PaintOnScreen, true);

    GLint size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);

#ifdef DEBUG_PICTUREITEM_GL
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureItemGL::initializeGL()" << "GL_MAX_TEXTURE_SIZE" << size;
#endif

    m_texImg->setTexMaxSize(size);
}

void PictureItemGL::updateSize()
{
    if (m_textures.size() == 0)
    {
        return;
    }

    m_data->updateSize(this->size());

    m_scaleX = (m_texImg->hTile->bmpSize * m_data->getZoom()) / this->width();
    m_scaleY = (m_texImg->vTile->bmpSize * m_data->getZoom()) / this->height();
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

    glTranslated(m_data->m_boundingRect.x() + m_data->m_offsetX + m_data->m_boundingRect.width() / 2,
                 m_data->m_boundingRect.y() + m_data->m_offsetY + m_data->m_boundingRect.height() / 2,
                 0);
    glRotated(m_data->getRotation(), 0, 0, 1);
    glTranslated(-(m_texImg->hTile->bmpSize * m_data->getZoom()) / 2,
                 -(m_texImg->vTile->bmpSize * m_data->getZoom()) / 2,
                 0);
    glScaled(m_scaleX, m_scaleY, 1);

    const QRectF texImage = QRectF(QPointF(0.0, 0.0), QPointF (1.0, 1.0));
    const QRectF vertImage = QRectF(QPointF(0, 0), QPointF (this->width(), this->height()));

    for (int hIndex = 0; hIndex < m_texImg->hTile->tileCount; ++hIndex)
    {
        double texScale = (double)m_texImg->hTile->bmpSize / (double)m_texImg->hTile->tileSize.at(hIndex);
        double tx;
        double tx2;
        double qx;
        double qx2;

        if (!ClipTextureVertex(texImage.left(), texImage.right(), vertImage.left(), vertImage.right(),
                               m_texImg->hTile->switchBorderNorm.at(hIndex), m_texImg->hTile->switchBorderNorm.at(hIndex + 1),
                               m_texImg->hTile->offsetBorderNorm.at(hIndex), texScale, tx, tx2, qx, qx2))
        {
            continue;
        }

        for (int vIndex = 0; vIndex < m_texImg->vTile->tileCount; ++vIndex)
        {
            texScale = (double)m_texImg->vTile->bmpSize / (double)m_texImg->vTile->tileSize.at(vIndex);
            double ty;
            double ty2;
            double qy;
            double qy2;

            if (!ClipTextureVertex(texImage.top(), texImage.bottom(), vertImage.top(), vertImage.bottom(),
                                   m_texImg->vTile->switchBorderNorm.at(vIndex), m_texImg->vTile->switchBorderNorm.at(vIndex + 1),
                                   m_texImg->vTile->offsetBorderNorm.at(vIndex), texScale, ty, ty2, qy, qy2))
            {
                continue;
            }

            glBindTexture(GL_TEXTURE_2D, m_textures.at(hIndex).at(vIndex));
            glBegin(GL_QUADS);

            glTexCoord2f(tx, ty);
            glVertex2f(qx, qy);
            glTexCoord2f(tx, ty2);
            glVertex2f(qx, qy2);
            glTexCoord2f(tx2, ty2);
            glVertex2f(qx2, qy2);
            glTexCoord2f(tx2, ty);
            glVertex2f(qx2, qy);
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

void PictureItemGL::setRotation(const qreal current, const qreal /*previous*/)
{
    if (qRound(current) % 360 == 0)
    {
        const qreal newWidth = m_texImg->hTile->bmpSize * m_data->getZoom();
        const qreal newHeight = m_texImg->vTile->bmpSize * m_data->getZoom();
        const QPointF p = m_data->pointToOrigin(newWidth, newHeight, this->size());
        m_data->m_boundingRect = QRectF(p.x(), p.y(), newWidth, newHeight);
    }
    else
    {
        QTransform tRot;
        tRot.translate(m_data->m_boundingRect.x(), m_data->m_boundingRect.y());
        tRot.scale(m_data->getZoom(), m_data->getZoom());
        tRot.translate((m_texImg->vTile->bmpSize / 2), (m_texImg->hTile->bmpSize / 2));
        tRot.rotate(current);
        tRot.translate((-m_texImg->vTile->bmpSize / 2), (-m_texImg->hTile->bmpSize / 2));
        const QRect transformedRot = tRot.mapRect(QRect(QPoint(0, 0), QSize(m_texImg->hTile->bmpSize, m_texImg->vTile->bmpSize)));

        const QPointF p = m_data->pointToOrigin(transformedRot.width(), transformedRot.height(), this->size());
        m_data->m_boundingRect = QRectF(p.x(), p.y(), transformedRot.width(), transformedRot.height());
    }

    m_data->avoidOutOfScreen(this->size());
    this->updateSize();
    this->updateGL();
}
#endif // QT5
