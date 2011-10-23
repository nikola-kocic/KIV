#include "pictureitem_gl.h"
#include "settings.h"

//#include <QtCore/qdebug.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtGui/qevent.h>

PictureItemGL::PictureItemGL(PictureItemShared* pis, QWidget * parent, Qt::WindowFlags f)
{
    this->pis = pis;
    connect(pis, SIGNAL(pixmapChanged()), this, SLOT(setPixmap()));
    connect(pis, SIGNAL(zoomChanged(qreal,qreal) ), this, SLOT(setZoom(qreal,qreal)));
    connect(pis, SIGNAL(update()), this, SLOT(update()));

    ti = new TexImg();

    textures = QVector < QVector < GLuint > >(0);
    clearColor = Qt::lightGray;
    offsetX = offsetY = 0;
    scaleY = scaleX = 0;
}

void PictureItemGL::setFile(const FileInfo &info)
{
    ti->UnloadPow2Bitmap();

    for (int hIndex = 0; hIndex < textures.count(); ++hIndex)
    {
        for(int vIndex=0; vIndex < textures.at(hIndex).count(); ++vIndex)
        {
            glDeleteTextures(1, &textures.at(hIndex).at(vIndex));
        }
    }

    if (info.imageFileName.isEmpty() && info.zipImageFileName.isEmpty())
    {
        this->textures = QVector < QVector < GLuint > >(0);
        pis->setPixmap(QPixmap(0, 0));
    }
    else
    {
        ti->CreatePow2Bitmap(info);
        this->textures = QVector < QVector < GLuint > >(ti->hTile->tileCount);
        for (int hIndex = 0; hIndex < ti->hTile->tileCount; ++hIndex)
        {
            textures[hIndex].resize(ti->vTile->tileCount);

            for(int vIndex=0; vIndex < ti->vTile->tileCount; ++vIndex)
            {
                glGenTextures(1, &textures[hIndex][vIndex]);
                glBindTexture(GL_TEXTURE_2D, textures[hIndex][vIndex]);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// GL_NEAREST is another choice
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ti->hTile->tileSize.at(hIndex), ti->vTile->tileSize.at(vIndex), 0,
                             GL_RGBA, GL_UNSIGNED_BYTE, ti->pow2TileBuffer.at(hIndex).at(vIndex));
            }
        }

        pis->setPixmap(QPixmap(1, 1));
    }
}

void PictureItemGL::setPixmap()
{
    if (pis->isPixmapNull())
    {
        updateGL();
        return;
    }

    setUpdatesEnabled(false);

    setRotation(0);
    if (pis->getLockMode() != LockMode::Zoom)
    {
        pis->setZoom(1);
    }

    pis->boundingRect = QRect(0, 0, (ti->hTile->bmpSize * pis->getZoom()), (ti->vTile->bmpSize * pis->getZoom()));

    pis->afterPixmapLoad();

    updateSize();
    setUpdatesEnabled(true);
    updateGL();
}

PictureItemGL::~PictureItemGL()
{
}

void PictureItemGL::setClearColor(const QColor &color)
{
    clearColor = color;
    updateGL();
}

void PictureItemGL::initializeGL()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    GLint size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
    ti->setTexMaxSize(size);
}

void PictureItemGL::updateSize()
{
    if (textures.count() == 0)
    {
        return;
    }

    offsetX = (pis->boundingRect.width() - this->width()) / 2;
    offsetY = (pis->boundingRect.height() - this->height()) / 2;

    scaleX = (ti->hTile->bmpSize * pis->getZoom()) / this->width();
    scaleY = (ti->vTile->bmpSize * pis->getZoom()) / this->height();
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
    double num2 = qMax(texCrd1, texBorder1);
    double num3 = qMin(texCrd2, texBorder2);
    bool result = num2 < num3;
    double num4 = 1.0 / (texCrd2 - texCrd1);
    double num5 = (num2 - texCrd1) * num4;
    double num6 = (num3 - texCrd1) * num4;
    double num7 = vertexCrd2 - vertexCrd1;
    vertexClip1 = vertexCrd1 + num5 * num7;
    vertexClip2 = vertexCrd2 - (1.0 - num6) * num7;
    texClip1 = (num2 - texOffsetMin) * texScale;
    texClip2 = (num3 - texOffsetMin) * texScale;
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

    if (textures.count() == 0)
    {
        return;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated((pis->boundingRect.x() + (offsetX > 0 ? offsetX : 0)), (pis->boundingRect.y() + (offsetY > 0 ? offsetY : 0)), 0);
    glRotated(pis->getRotation(), 0 , 0, 1);
    glScaled(scaleX, scaleY, 1);

    QRectF tImage = QRectF(QPointF(0.0, 0.0), QPointF (1.0, 1.0));
    QRectF vImage = QRectF(QPointF(-this->width() * 0.5, -this->height() * 0.5), QPointF (this->width() * 0.5, this->height() * 0.5));

    for(int hIndex=0; hIndex<ti->hTile->tileCount; ++hIndex)
    {
        double texScale = (double)ti->hTile->bmpSize / (double)ti->hTile->tileSize.at(hIndex);
        double tx;
        double tx2;
        double qx;
        double qx2;
        bool flag = ClipTextureVertex(tImage.left(), tImage.right(), vImage.left(), vImage.right(),
                                      ti->hTile->switchBorderNorm.at(hIndex), ti->hTile->switchBorderNorm.at(hIndex + 1),
                                      ti->hTile->offsetBorderNorm.at(hIndex), texScale, tx, tx2, qx, qx2);
        if (flag)
        {
            for(int vIndex=0; vIndex<ti->vTile->tileCount; ++vIndex)
            {
                texScale = (double)ti->vTile->bmpSize / (double)ti->vTile->tileSize.at(vIndex);
                double ty;
                double ty2;
                double qy;
                double qy2;
                flag = ClipTextureVertex(tImage.top(), tImage.bottom(), vImage.top(), vImage.bottom(),
                                         ti->vTile->switchBorderNorm.at(vIndex), ti->vTile->switchBorderNorm.at(vIndex + 1),
                                         ti->vTile->offsetBorderNorm.at(vIndex), texScale, ty, ty2, qy, qy2);
                if (flag)
                {
                    glBindTexture(GL_TEXTURE_2D, textures.at(hIndex).at(vIndex));
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


//Region Rotation

void PictureItemGL::setRotation(qreal r)
{
    if (textures.count() == 0)
    {
        return;
    }

    pis->setRotation(r);

    QTransform tRot;
    tRot.translate(pis->boundingRect.x(), pis->boundingRect.y());
    tRot.scale(pis->getZoom(), pis->getZoom());
    tRot.translate((ti->vTile->bmpSize / 2), (ti->hTile->bmpSize / 2));
    tRot.rotate(pis->getRotation());
    tRot.translate((-ti->vTile->bmpSize / 2), (-ti->hTile->bmpSize / 2));
    QRect transformedRot = tRot.mapRect(QRect(QPoint(0, 0), QSize(ti->hTile->bmpSize, ti->vTile->bmpSize)));

    pis->boundingRect.setWidth(transformedRot.width());
    pis->boundingRect.setHeight(transformedRot.height());

    if ((pis->boundingRect.height() + pis->boundingRect.y() ) < this->height() )
    {
        pis->boundingRect.translate(0, (this->height() - (pis->boundingRect.height() + pis->boundingRect.y())));
    }

    if (pis->boundingRect.height() < this->height())
    {
        pis->boundingRect.moveTop(0);
    }

    updateSize();
    updateGL();
}


//End Region Rotation

//Region Zoom

void PictureItemGL::setZoom(qreal current, qreal previous)
{
    if (textures.count() == 0)
    {
        return;
    }

    qreal scaledW = (pis->boundingRect.width() / previous) * current;
    qreal scaledH = (pis->boundingRect.height() / previous) * current;
    QPointF p = pis->pointToOrigin(scaledW, scaledH);
    pis->boundingRect = QRectF(p.x(), p.y(), scaledW, scaledH);

    setUpdatesEnabled(false);
    pis->avoidOutOfScreen();
    setRotation(pis->getRotation());
    setUpdatesEnabled(true);

    updateGL();

    emit zoomChanged();
}
