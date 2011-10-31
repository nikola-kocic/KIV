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
    connect(this->picItemShared, SIGNAL(zoomChanged(qreal,qreal)), this, SLOT(setZoom(qreal,qreal)));
    connect(this->picItemShared, SIGNAL(update()), this, SLOT(update()));

    this->ti = new TexImg();

    this->textureLoader = new QFutureWatcher<QImage>(this);
    connect(this->textureLoader, SIGNAL(resultReadyAt(int)), this, SLOT(textureFinished(int)));

    this->imageLoader = new QFutureWatcher<QImage>(this);
    connect(this->imageLoader, SIGNAL(resultReadyAt(int)), this, SLOT(imageFinished(int)));

    this->clearColor = Qt::lightGray;
    this->offsetX = offsetY = 0;
    this->scaleY = scaleX = 0;
    this->textures = QVector < QVector < GLuint > >(0);
}

PictureItemGL::~PictureItemGL()
{
    clearTextures();
    delete this->ti;
}

void PictureItemGL::clearTextures()
{
    //Delete old textures
//    qDebug() << "textures.count()" << textures.count();
    for (int hIndex = 0; hIndex < this->textures.count(); ++hIndex)
    {
//        qDebug() << "this->textures.at(hIndex).count()" << this->textures.at(hIndex).count();
        for (int vIndex = 0; vIndex < this->textures.at(hIndex).count(); ++vIndex)
        {
            glDeleteTextures(1, &this->textures.at(hIndex).at(vIndex));
//            qDebug() << "deleted texture @" << hIndex << vIndex;
        }
    }
}

void PictureItemGL::imageFinished(int num)
{
    this->ti->setImage(this->imageLoader->resultAt(num));

    //Free result memory
    this->imageLoader->setFuture(QFuture<QImage>());

    clearTextures();
    this->textures = QVector < QVector < GLuint > >(this->ti->hTile->tileCount);
    QList<TexIndex> indexes;
    this->returnTexCount = 0;
    for (int hIndex = 0; hIndex < this->ti->hTile->tileCount; ++hIndex)
    {
        this->textures[hIndex].resize(this->ti->vTile->tileCount);

        for (int vIndex = 0; vIndex < this->ti->vTile->tileCount; ++vIndex)
        {
            TexIndex tex;
            tex.bitmapData = this->ti->bitmapData;
            tex.currentTileWidth = this->ti->hTile->tileSize.at(hIndex);
            tex.currentTileHeight = this->ti->vTile->tileSize.at(vIndex);
            tex.hBorderOffset = this->ti->hTile->offsetBorder.at(hIndex);
            tex.vBorderOffset = this->ti->vTile->offsetBorder.at(vIndex);
            indexes.append(tex);
        }
    }

    this->textureLoader->setFuture(QtConcurrent::mapped(indexes, TexImg::CreatePow2Bitmap));
}

void PictureItemGL::textureFinished(int num)
{
    this->setUpdatesEnabled(false);

    int hIndex = num / this->ti->vTile->tileCount;
    int vIndex = num % this->ti->vTile->tileCount;

    this->textures[hIndex][vIndex] = bindTexture(this->textureLoader->resultAt(num), GL_TEXTURE_2D, GL_RGB, QGLContext::LinearFilteringBindOption | QGLContext::MipmapBindOption);

    if (++this->returnTexCount == (this->ti->hTile->tileCount * this->ti->vTile->tileCount))
    {
        this->returnTexCount = 0;

        //Free memory
        this->ti->bitmapData = QImage();
        this->textureLoader->setFuture(QFuture<QImage>());

        //Update view
        this->picItemShared->setPixmapNull(false);

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
}

void PictureItemGL::setFile(const FileInfo &info)
{
    //If file is null, display nothing
    if (info.imageFileName.isEmpty() && info.zipImageFileName.isEmpty())
    {
//        qDebug() << "textures = 0";
        clearTextures();
        this->ti->setImage(QImage());

//        this->imageLoader->setFuture(QFuture<QImage>());
//        this->textureLoader->setFuture(QFuture<QImage>());
//        this->textures.clear();

        this->textures = QVector < QVector < GLuint > >(0);
        this->picItemShared->setPixmapNull(true);
        this->updateGL();
        emit imageChanged();
    }
    else
    {
        this->imageLoader->setFuture(QtConcurrent::run(PictureLoader::getImage, info));
    }
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

    for (int hIndex = 0; hIndex < this->ti->hTile->tileCount; ++hIndex)
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

        for (int vIndex = 0; vIndex < this->ti->vTile->tileCount; ++vIndex)
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

    if ((this->picItemShared->boundingRect.height() + this->picItemShared->boundingRect.y()) < this->height())
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
