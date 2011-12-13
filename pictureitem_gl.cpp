#include "pictureitem.h"
#include "settings.h"
#include "picture_loader.h"

#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtGui/qevent.h>
#include <QtOpenGL/qglframebufferobject.h>

//#define DEBUG_PICTUREITEM_GL

#ifdef DEBUG_PICTUREITEM_GL
    #include <QtCore/qdebug.h>
#endif

PictureItem::PictureItemGL::PictureItemGL(PictureItem *parent, Qt::WindowFlags f)
{
    this->picItem = parent;

    this->texImg = new TexImg();

    this->offsetX = offsetY = 0;
    this->scaleY = scaleX = 0;
    this->textures = QVector < QVector < GLuint > >(0);
}

PictureItem::PictureItemGL::~PictureItemGL()
{
    old_textures = textures;
    clearTextures();
    delete this->texImg;
}

void PictureItem::PictureItemGL::clearTextures()
{
    /* Delete old textures */
    for (int hIndex = 0; hIndex < this->old_textures.size(); ++hIndex)
    {
        for (int vIndex = 0; vIndex < this->old_textures.at(hIndex).size(); ++vIndex)
        {
            deleteTexture(this->old_textures.at(hIndex).at(vIndex));
#ifdef DEBUG_PICTUREITEM_GL
            qDebug() << QDateTime::currentDateTime() << "deleted texture" << this->old_textures.at(hIndex).at(vIndex) << "@" << hIndex << vIndex;
#endif
        }
    }
}

void PictureItem::PictureItemGL::setImage(QImage img)
{
    old_textures = textures;
    if (img.isNull())
    {
        clearTextures();
        this->texImg->setImage(QImage());

        this->textures = QVector < QVector < GLuint > >(0);
        this->updateGL();
        return;
    }

    this->setUpdatesEnabled(false);

    if (Settings::Instance()->getCalculateAverageColor())
    {
        QImage averageColorImage = img.scaled(1,1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        setClearColor(QColor::fromRgb(averageColorImage.pixel(0,0)));
    }
    else
    {
        if (picItem->clearColor != Qt::lightGray)
        {
            setClearColor(Qt::lightGray);
        }
    }

    this->texImg->setImage(img);

    this->textures = QVector < QVector < GLuint > >(this->texImg->hTile->tileCount);
    QList<TexIndex> indexes;
    for (int hIndex = 0; hIndex < this->texImg->hTile->tileCount; ++hIndex)
    {
        this->textures[hIndex].resize(this->texImg->vTile->tileCount);

        for (int vIndex = 0; vIndex < this->texImg->vTile->tileCount; ++vIndex)
        {
            TexIndex tex;
            tex.bitmapData = img;
            tex.background = picItem->clearColor;
            tex.currentTileWidth = this->texImg->hTile->tileSize.at(hIndex);
            tex.currentTileHeight = this->texImg->vTile->tileSize.at(vIndex);
            tex.hBorderOffset = this->texImg->hTile->offsetBorder.at(hIndex);
            tex.vBorderOffset = this->texImg->vTile->offsetBorder.at(vIndex);
            indexes.append(tex);
        }
    }

    this->picItem->loadTextures(indexes);
    clearTextures();
}

void PictureItem::PictureItemGL::setTexture(QImage tex, int num)
{
    int hIndex = num / this->texImg->vTile->tileCount;
    int vIndex = num % this->texImg->vTile->tileCount;

    this->textures[hIndex][vIndex] = bindTexture(tex, GL_TEXTURE_2D, GL_RGB, QGLContext::LinearFilteringBindOption | QGLContext::MipmapBindOption);

#ifdef DEBUG_PICTUREITEM_GL
    qDebug() << QDateTime::currentDateTime() << "bound texture" << this->textures.at(hIndex).at(vIndex) << "@" << hIndex << vIndex <<";" << tex.size();
#endif
}

void PictureItem::PictureItemGL::textureLoadFinished()
{
    /* Update view */

    this->picItem->setRotation(0);
    if (this->picItem->getLockMode() != LockMode::Zoom)
    {
        this->picItem->setZoom(1);
    }

    this->picItem->boundingRect = QRect(0, 0, (this->texImg->hTile->bmpSize * this->picItem->getZoom()), (this->texImg->vTile->bmpSize * this->picItem->getZoom()));
    this->picItem->afterPixmapLoad();

    this->updateSize();

    this->setUpdatesEnabled(true);
    this->updateGL();
}


void PictureItem::PictureItemGL::setClearColor(const QColor &color)
{
    picItem->clearColor = color;
    qglClearColor(picItem->clearColor);
    this->updateGL();
}

void PictureItem::PictureItemGL::initializeGL()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    qglClearColor(picItem->clearColor);

    uint _glFormat = GL_RGB;  // Better since QImage RGBA is BGRA

    QGL::setPreferredPaintEngine(QPaintEngine::OpenGL2);

    QGLFormat glFmt;
    glFmt.setSwapInterval(1); // 1= vsync on
    glFmt.setAlpha(GL_RGBA==_glFormat);
    glFmt.setRgba(GL_RGBA==_glFormat);
    glFmt.setDoubleBuffer(true); // default
    glFmt.setOverlay(false);
    glFmt.setSampleBuffers(false);
    QGLFormat::setDefaultFormat(glFmt);

    setAttribute(Qt::WA_OpaquePaintEvent,true);
    setAttribute(Qt::WA_PaintOnScreen,true);

    GLint size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
    this->texImg->setTexMaxSize(size);
}

void PictureItem::PictureItemGL::updateSize()
{
    if (this->textures.size() == 0)
    {
        return;
    }

    this->offsetX = (this->picItem->boundingRect.width() - this->width()) / 2;
    this->offsetY = (this->picItem->boundingRect.height() - this->height()) / 2;

    this->scaleX = (this->texImg->hTile->bmpSize * this->picItem->getZoom()) / this->width();
    this->scaleY = (this->texImg->vTile->bmpSize * this->picItem->getZoom()) / this->height();
}

void PictureItem::PictureItemGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (this->textures.size() == 0)
    {
        return;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated((this->picItem->boundingRect.x() + (this->offsetX > 0 ? this->offsetX : 0)), (this->picItem->boundingRect.y() + (this->offsetY > 0 ? this->offsetY : 0)), 0);
    glRotated(this->picItem->getRotation(), 0 , 0, 1);
    glScaled(this->scaleX, this->scaleY, 1);

    QRectF texImage = QRectF(QPointF(0.0, 0.0), QPointF (1.0, 1.0));
    QRectF vertImage = QRectF(QPointF(-this->width() * 0.5, -this->height() * 0.5), QPointF (this->width() * 0.5, this->height() * 0.5));

    for (int hIndex = 0; hIndex < this->texImg->hTile->tileCount; ++hIndex)
    {
        double texScale = (double)this->texImg->hTile->bmpSize / (double)this->texImg->hTile->tileSize.at(hIndex);
        double tx;
        double tx2;
        double qx;
        double qx2;

        if (!ClipTextureVertex(texImage.left(), texImage.right(), vertImage.left(), vertImage.right(),
                               this->texImg->hTile->switchBorderNorm.at(hIndex), this->texImg->hTile->switchBorderNorm.at(hIndex + 1),
                               this->texImg->hTile->offsetBorderNorm.at(hIndex), texScale, tx, tx2, qx, qx2))
        {
            continue;
        }

        for (int vIndex = 0; vIndex < this->texImg->vTile->tileCount; ++vIndex)
        {
            texScale = (double)this->texImg->vTile->bmpSize / (double)this->texImg->vTile->tileSize.at(vIndex);
            double ty;
            double ty2;
            double qy;
            double qy2;

            if (!ClipTextureVertex(texImage.top(), texImage.bottom(), vertImage.top(), vertImage.bottom(),
                                   this->texImg->vTile->switchBorderNorm.at(vIndex), this->texImg->vTile->switchBorderNorm.at(vIndex + 1),
                                   this->texImg->vTile->offsetBorderNorm.at(vIndex), texScale, ty, ty2, qy, qy2))
            {
                continue;
            }

            glBindTexture(GL_TEXTURE_2D, this->textures.at(hIndex).at(vIndex));
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
    glOrtho((-width / 2), (width / 2), (height / 2), (-height / 2), 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void PictureItem::PictureItemGL::setRotation(qreal r)
{
    QTransform tRot;
    tRot.translate(this->picItem->boundingRect.x(), this->picItem->boundingRect.y());
    tRot.scale(this->picItem->getZoom(), this->picItem->getZoom());
    tRot.translate((this->texImg->vTile->bmpSize / 2), (this->texImg->hTile->bmpSize / 2));
    tRot.rotate(this->picItem->getRotation());
    tRot.translate((-this->texImg->vTile->bmpSize / 2), (-this->texImg->hTile->bmpSize / 2));
    QRect transformedRot = tRot.mapRect(QRect(QPoint(0, 0), QSize(this->texImg->hTile->bmpSize, this->texImg->vTile->bmpSize)));

    this->picItem->boundingRect.setWidth(transformedRot.width());
    this->picItem->boundingRect.setHeight(transformedRot.height());

    if ((this->picItem->boundingRect.height() + this->picItem->boundingRect.y()) < this->height())
    {
        this->picItem->boundingRect.translate(0, (this->height() - (this->picItem->boundingRect.height() + this->picItem->boundingRect.y())));
    }

    if (this->picItem->boundingRect.height() < this->height())
    {
        this->picItem->boundingRect.moveTop(0);
    }

    this->updateSize();
    this->updateGL();
}

void PictureItem::PictureItemGL::setZoom(qreal current, qreal previous)
{
    qreal scaledW = (this->picItem->boundingRect.width() / previous) * current;
    qreal scaledH = (this->picItem->boundingRect.height() / previous) * current;
    QPointF p = this->picItem->pointToOrigin(scaledW, scaledH);
    this->picItem->boundingRect = QRectF(p.x(), p.y(), scaledW, scaledH);

    this->picItem->avoidOutOfScreen();
    this->setRotation(this->picItem->getRotation());

    this->updateGL();
}
