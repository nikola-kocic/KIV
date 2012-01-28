#include "pictureitem.h"
#include "settings.h"
#include "picture_loader.h"

//#define DEBUG_PICTUREITEM_GL

#ifdef DEBUG_PICTUREITEM_GL
#include <QDebug>
#include <QDateTime>
#endif
PictureItem::PictureItemGL::PictureItemGL(const QGLFormat& format, PictureItem *parent, const QGLWidget* shareWidget, Qt::WindowFlags f)
    : QGLWidget(format, parent, shareWidget, f)
    , m_picItem(parent)
    , m_scaleX(0)
    , m_scaleY(0)
    , m_offsetX(0)
    , m_offsetY(0)
    , m_textures(QVector < QVector < GLuint > >(0))
    , m_old_textures(QVector < QVector < GLuint > >(0))
    , m_texImg(new TexImg())
{
}

PictureItem::PictureItemGL::~PictureItemGL()
{
    m_old_textures = m_textures;
    clearTextures();
    delete m_texImg;
}

void PictureItem::PictureItemGL::clearTextures()
{
    /* Delete old textures */
    for (int hIndex = 0; hIndex < m_old_textures.size(); ++hIndex)
    {
        for (int vIndex = 0; vIndex < m_old_textures.at(hIndex).size(); ++vIndex)
        {
            deleteTexture(m_old_textures.at(hIndex).at(vIndex));
#ifdef DEBUG_PICTUREITEM_GL
            qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "deleted texture" << this->m_old_textures.at(hIndex).at(vIndex) << "@" << hIndex << vIndex;
#endif
        }
    }

    m_old_textures = QVector < QVector < GLuint > >(0);
}

void PictureItem::PictureItemGL::setImage(const QImage &img)
{
    m_old_textures = m_textures;
    if (img.isNull())
    {
        clearTextures();
        m_texImg->setImage(QImage());

        m_textures = QVector < QVector < GLuint > >(0);
        this->updateGL();
        return;
    }

    this->setUpdatesEnabled(false);

    m_texImg->setImage(img);

    m_textures = QVector < QVector < GLuint > >(m_texImg->hTile->tileCount);
    QList<TexIndex> indexes;
    for (int hIndex = 0; hIndex < m_texImg->hTile->tileCount; ++hIndex)
    {
        m_textures[hIndex].resize(m_texImg->vTile->tileCount);

        for (int vIndex = 0; vIndex < m_texImg->vTile->tileCount; ++vIndex)
        {
            TexIndex tex;
            tex.bitmapData = img;
            tex.background = m_picItem->m_color_clear;
            tex.currentTileWidth = m_texImg->hTile->tileSize.at(hIndex);
            tex.currentTileHeight = m_texImg->vTile->tileSize.at(vIndex);
            tex.hBorderOffset = m_texImg->hTile->offsetBorder.at(hIndex);
            tex.vBorderOffset = m_texImg->vTile->offsetBorder.at(vIndex);
            indexes.append(tex);
        }
    }

    m_picItem->loadTextures(indexes);
    clearTextures();
}

void PictureItem::PictureItemGL::setTexture(const QImage &tex, const int num)
{
    const int hIndex = num / m_texImg->vTile->tileCount;
    const int vIndex = num % m_texImg->vTile->tileCount;

    m_textures[hIndex][vIndex] = bindTexture(tex, GL_TEXTURE_2D, GL_RGB, QGLContext::LinearFilteringBindOption | QGLContext::MipmapBindOption);

#ifdef DEBUG_PICTUREITEM_GL
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "bound texture" << this->m_textures.at(hIndex).at(vIndex) << "@" << hIndex << vIndex <<";" << tex.size();
#endif
}

void PictureItem::PictureItemGL::textureLoadFinished()
{
    /* Update view */

    m_picItem->setRotation(0);
    if (m_picItem->getLockMode() != LockMode::Zoom)
    {
        m_picItem->setZoom(1);
    }

    m_picItem->m_boundingRect = QRect(0, 0, (m_texImg->hTile->bmpSize * m_picItem->getZoom()), (m_texImg->vTile->bmpSize * m_picItem->getZoom()));
    m_picItem->afterPixmapLoad();

    this->updateSize();

    this->setUpdatesEnabled(true);
    this->updateGL();
}


void PictureItem::PictureItemGL::updateClearColor()
{
    qglClearColor(m_picItem->m_color_clear);
    this->updateGL();
}

void PictureItem::PictureItemGL::initializeGL()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    qglClearColor(m_picItem->m_color_clear);


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

void PictureItem::PictureItemGL::updateSize()
{
    if (m_textures.size() == 0)
    {
        return;
    }

    if (this->width() > m_picItem->m_boundingRect.width())
    {
        if (m_picItem->getZoom() == 1)
        {
            m_offsetX = qRound((this->width() - m_picItem->m_boundingRect.width()) / 2);
        }
        else
        {
            m_offsetX = (this->width() - m_picItem->m_boundingRect.width()) / 2;
        }
    }
    else
    {
        m_offsetX = 0;
    }

    if (this->height() > m_picItem->m_boundingRect.height())
    {
        if (m_picItem->getZoom() == 1)
        {
            m_offsetY = qRound((this->height() - m_picItem->m_boundingRect.height()) / 2);
        }
        else
        {
            m_offsetY = (this->height() - m_picItem->m_boundingRect.height()) / 2;
        }
    }
    else
    {
        m_offsetY = 0;
    }

    m_scaleX = (m_texImg->hTile->bmpSize * m_picItem->getZoom()) / this->width();
    m_scaleY = (m_texImg->vTile->bmpSize * m_picItem->getZoom()) / this->height();
}

void PictureItem::PictureItemGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_textures.size() == 0)
    {
        return;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated(m_picItem->m_boundingRect.x() + m_offsetX, m_picItem->m_boundingRect.y() + m_offsetY, 0);
    glRotated(m_picItem->getRotation(), 0 , 0, 1);
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

void PictureItem::PictureItemGL::resizeGL(int width, int height)
{
    updateSize();
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void PictureItem::PictureItemGL::setRotation(const qreal r)
{
    QTransform tRot;
    tRot.translate(m_picItem->m_boundingRect.x(), m_picItem->m_boundingRect.y());
    tRot.scale(m_picItem->getZoom(), m_picItem->getZoom());
    tRot.translate((m_texImg->vTile->bmpSize / 2), (m_texImg->hTile->bmpSize / 2));
    tRot.rotate(r);
    tRot.translate((-m_texImg->vTile->bmpSize / 2), (-m_texImg->hTile->bmpSize / 2));
    const QRect transformedRot = tRot.mapRect(QRect(QPoint(0, 0), QSize(m_texImg->hTile->bmpSize, m_texImg->vTile->bmpSize)));

    m_picItem->m_boundingRect.setWidth(transformedRot.width());
    m_picItem->m_boundingRect.setHeight(transformedRot.height());

    if ((m_picItem->m_boundingRect.height() + m_picItem->m_boundingRect.y()) < this->height())
    {
        m_picItem->m_boundingRect.translate(0, (this->height() - (m_picItem->m_boundingRect.height() + m_picItem->m_boundingRect.y())));
    }

    if (m_picItem->m_boundingRect.height() < this->height())
    {
        m_picItem->m_boundingRect.moveTop(0);
    }

    this->updateSize();
    this->updateGL();
}

void PictureItem::PictureItemGL::setZoom(const qreal current, const qreal previous)
{
    const qreal scaledW = (m_picItem->m_boundingRect.width() / previous) * current;
    const qreal scaledH = (m_picItem->m_boundingRect.height() / previous) * current;
    const QPointF p = m_picItem->pointToOrigin(scaledW, scaledH);
    m_picItem->m_boundingRect = QRectF(p.x(), p.y(), scaledW, scaledH);

    m_picItem->avoidOutOfScreen();
    this->setRotation(m_picItem->getRotation());

    this->updateGL();
}
