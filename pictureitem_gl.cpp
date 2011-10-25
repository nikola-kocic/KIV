#include "pictureitem_gl.h"
#include "settings.h"
#include "picture_loader.h"

//#include <QtCore/qdebug.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtGui/qevent.h>

PictureItemGL::PictureItemGL(PictureItemShared *picItemShared, QWidget *parent, Qt::WindowFlags f)
{
    this->picItemShared = picItemShared;
    connect(this->picItemShared, SIGNAL(zoomChanged(qreal,qreal) ), this, SLOT(setZoom(qreal,qreal)));
    connect(this->picItemShared, SIGNAL(update()), this, SLOT(update()));

    this->ti = new TexImg();
    this->textures = QVector < QVector < GLuint > >(0);
    this->clearColor = Qt::lightGray;
    this->offsetX = offsetY = 0;
    this->scaleY = scaleX = 0;
}

void PictureItemGL::setFile(const FileInfo &info)
{
    this->setUpdatesEnabled(false);

    //Delete old textures

    this->ti->UnloadPow2Bitmap();

    for (int hIndex = 0; hIndex < textures.count(); ++hIndex)
    {
        for (int vIndex=0; vIndex < textures.at(hIndex).count(); ++vIndex)
        {
            glDeleteTextures(1, &textures.at(hIndex).at(vIndex));
        }
    }

    //If file is null, display nothing
    if (info.imageFileName.isEmpty() && info.zipImageFileName.isEmpty())
    {
        this->textures = QVector < QVector < GLuint > >(0);
        picItemShared->setPixmapNull(true);
        this->updateGL();
        emit imageChanged();
        return;
    }


    //Create textures in memory
    this->ti->CreatePow2Bitmap(info);

    //Upload textures from memory
    this->textures = QVector < QVector < GLuint > >(this->ti->hTile->tileCount);
    for (int hIndex = 0; hIndex < this->ti->hTile->tileCount; ++hIndex)
    {
        this->textures[hIndex].resize(this->ti->vTile->tileCount);

        for (int vIndex=0; vIndex < this->ti->vTile->tileCount; ++vIndex)
        {
            glGenTextures(1, &textures[hIndex][vIndex]);
            glBindTexture(GL_TEXTURE_2D, textures[hIndex][vIndex]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// GL_NEAREST is another choice
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->ti->hTile->tileSize.at(hIndex), this->ti->vTile->tileSize.at(vIndex), 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, this->ti->pow2TileBuffer.at(hIndex).at(vIndex));
        }
    }

    //Clear textures from memory
    for (int hIndex = 0; hIndex < this->ti->pow2TileBuffer.count(); ++hIndex)
    {
        for (int vIndex=0; vIndex < this->ti->pow2TileBuffer.at(hIndex).count(); ++vIndex)
        {
            delete this->ti->pow2TileBuffer.at(hIndex).at(vIndex);
        }
    }
    this->ti->pow2TileBuffer.clear();


    //Update view
    picItemShared->setPixmapNull(false);

    this->setRotation(0);
    if (this->picItemShared->getLockMode() != LockMode::Zoom)
    {
        this->picItemShared->setZoom(1);
    }

    this->picItemShared->boundingRect = QRect(0, 0, (this->ti->hTile->bmpSize * this->picItemShared->getZoom()), (this->ti->vTile->bmpSize * this->picItemShared->getZoom()));
    this->picItemShared->afterPixmapLoad();

    this->updateSize();

    this->setUpdatesEnabled(true);
    this->updateGL();
    emit imageChanged();
}

void PictureItemGL::setClearColor(const QColor &color)
{
    this->clearColor = color;
    this->updateGL();
}

void PictureItemGL::initializeGL()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    GLint size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
    this->ti->setTexMaxSize(size);
}

void PictureItemGL::updateSize()
{
    if (this->textures.count() == 0)
    {
        return;
    }

    this->offsetX = (this->picItemShared->boundingRect.width() - this->width()) / 2;
    this->offsetY = (this->picItemShared->boundingRect.height() - this->height()) / 2;

    this->scaleX = (this->ti->hTile->bmpSize * this->picItemShared->getZoom()) / this->width();
    this->scaleY = (this->ti->vTile->bmpSize * this->picItemShared->getZoom()) / this->height();
}

bool ClipTextureVertex(double texCrd1, double texCrd2, double vertexCrd1, double vertexCrd2, double texBorder1, double texBorder2, double texOffsetMin, double texScale,
                       double &texClip1, double &texClip2, double &vertexClip1, double &vertexClip2)
{
    bool flag = false;
    if (texCrd1 > texCrd2)
    {
        flag = true;
        double num = texCrd1;
        texCrd1 = texCrd2;
        texCrd2 = num;
        num = vertexCrd1;
        vertexCrd1 = vertexCrd2;
        vertexCrd2 = num;
    }
    double coord1 = qMax(texCrd1, texBorder1);
    double coord2 = qMin(texCrd2, texBorder2);
    bool result = coord1 < coord2;
    double num4 = 1.0 / (texCrd2 - texCrd1);
    double num5 = (coord1 - texCrd1) * num4;
    double num6 = (coord2 - texCrd1) * num4;
    double num7 = vertexCrd2 - vertexCrd1;
    vertexClip1 = vertexCrd1 + num5 * num7;
    vertexClip2 = vertexCrd2 - (1.0 - num6) * num7;
    texClip1 = (coord1 - texOffsetMin) * texScale;
    texClip2 = (coord2 - texOffsetMin) * texScale;
    if (flag)
    {
        double num = texClip1;
        texClip1 = texClip2;
        texClip2 = num;
        num = vertexClip1;
        vertexClip1 = vertexClip2;
        vertexClip2 = num;
    }
    return result;
}

void PictureItemGL::paintGL()
{
    qglClearColor(clearColor);
    glClear(GL_COLOR_BUFFER_BIT);

    if (this->textures.count() == 0)
    {
        return;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated((this->picItemShared->boundingRect.x() + (this->offsetX > 0 ? this->offsetX : 0)), (this->picItemShared->boundingRect.y() + (this->offsetY > 0 ? this->offsetY : 0)), 0);
    glRotated(this->picItemShared->getRotation(), 0 , 0, 1);
    glScaled(this->scaleX, this->scaleY, 1);

    QRectF texImage = QRectF(QPointF(0.0, 0.0), QPointF (1.0, 1.0));
    QRectF vertImage = QRectF(QPointF(-this->width() * 0.5, -this->height() * 0.5), QPointF (this->width() * 0.5, this->height() * 0.5));

    for (int hIndex=0; hIndex<this->ti->hTile->tileCount; ++hIndex)
    {
        double texScale = (double)this->ti->hTile->bmpSize / (double)this->ti->hTile->tileSize.at(hIndex);
        double tx;
        double tx2;
        double qx;
        double qx2;

        if (!ClipTextureVertex(texImage.left(), texImage.right(), vertImage.left(), vertImage.right(),
                               this->ti->hTile->switchBorderNorm.at(hIndex), this->ti->hTile->switchBorderNorm.at(hIndex + 1),
                               this->ti->hTile->offsetBorderNorm.at(hIndex), texScale, tx, tx2, qx, qx2))
        {
            continue;
        }

        for (int vIndex=0; vIndex<this->ti->vTile->tileCount; ++vIndex)
        {
            texScale = (double)this->ti->vTile->bmpSize / (double)this->ti->vTile->tileSize.at(vIndex);
            double ty;
            double ty2;
            double qy;
            double qy2;

            if (!ClipTextureVertex(texImage.top(), texImage.bottom(), vertImage.top(), vertImage.bottom(),
                                   this->ti->vTile->switchBorderNorm.at(vIndex), this->ti->vTile->switchBorderNorm.at(vIndex + 1),
                                   this->ti->vTile->offsetBorderNorm.at(vIndex), texScale, ty, ty2, qy, qy2))
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

void PictureItemGL::resizeGL(int width, int height)
{
    updateSize();
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho((-width / 2), (width / 2), (height / 2), (-height / 2), 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void PictureItemGL::setRotation(qreal r)
{
    if (this->textures.count() == 0)
    {
        return;
    }

    this->picItemShared->setRotation(r);

    QTransform tRot;
    tRot.translate(this->picItemShared->boundingRect.x(), this->picItemShared->boundingRect.y());
    tRot.scale(this->picItemShared->getZoom(), this->picItemShared->getZoom());
    tRot.translate((this->ti->vTile->bmpSize / 2), (this->ti->hTile->bmpSize / 2));
    tRot.rotate(this->picItemShared->getRotation());
    tRot.translate((-this->ti->vTile->bmpSize / 2), (-this->ti->hTile->bmpSize / 2));
    QRect transformedRot = tRot.mapRect(QRect(QPoint(0, 0), QSize(this->ti->hTile->bmpSize, this->ti->vTile->bmpSize)));

    this->picItemShared->boundingRect.setWidth(transformedRot.width());
    this->picItemShared->boundingRect.setHeight(transformedRot.height());

    if ((this->picItemShared->boundingRect.height() + this->picItemShared->boundingRect.y() ) < this->height() )
    {
        this->picItemShared->boundingRect.translate(0, (this->height() - (this->picItemShared->boundingRect.height() + this->picItemShared->boundingRect.y())));
    }

    if (this->picItemShared->boundingRect.height() < this->height())
    {
        this->picItemShared->boundingRect.moveTop(0);
    }

    this->updateSize();
    this->updateGL();
}

void PictureItemGL::setZoom(qreal current, qreal previous)
{
    if (this->textures.count() == 0)
    {
        return;
    }

    qreal scaledW = (this->picItemShared->boundingRect.width() / previous) * current;
    qreal scaledH = (this->picItemShared->boundingRect.height() / previous) * current;
    QPointF p = this->picItemShared->pointToOrigin(scaledW, scaledH);
    this->picItemShared->boundingRect = QRectF(p.x(), p.y(), scaledW, scaledH);

    this->setUpdatesEnabled(false);
    this->picItemShared->avoidOutOfScreen();
    this->setRotation(this->picItemShared->getRotation());
    this->setUpdatesEnabled(true);

    this->updateGL();

    emit zoomChanged();
}
