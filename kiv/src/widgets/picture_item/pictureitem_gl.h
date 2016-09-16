#ifndef PICTUREITEM_GL_H
#define PICTUREITEM_GL_H

#include "kiv/src/widgets/picture_item/pictureitem_interface.h"

#include <QFutureWatcher>
#include <QtOpenGL/QGLWidget>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLTexture>
#include <QtConcurrent/QtConcurrentMap>

#include "kiv/src/widgets/picture_item/teximg.h"
#include "kiv/src/enums.h"

class PictureItemGL : private QGLWidget, public PictureItemInterface, protected QOpenGLFunctions_2_0
{
    Q_OBJECT

public:
    explicit PictureItemGL(PictureItemData *data, ZoomFilter zoomFilter, QWidget *parent);
    ~PictureItemGL() override;
    void setRotation(const qreal current, const qreal previous) override;
    void setBackgroundColor(const QColor &color) override;
    void setImage(const QImage &img) override;
    void setNullImage() override;
    void setTexture(const QImage &tex, const int num);
    void setZoom(const qreal current, const qreal previous) override;
    QWidget *getWidget();

private:
    void loadTextures(QList<TexIndex *> indexes);
    void updateSize();
    void clearTextures();

    QFutureWatcher< QImage > *m_loader_texture;
    qreal m_scaleX;
    qreal m_scaleY;
    QVector < QVector <QOpenGLTexture*> > m_textures;
    TexImg *m_texImg;
    int m_returnTexCount;
    ZoomFilter m_zoomFilter;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

private slots:
    void textureFinished(int num);
};

#endif  // PICTUREITEM_GL_H
