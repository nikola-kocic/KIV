#ifndef QT5
#ifndef PICTUREITEM_GL_H
#define PICTUREITEM_GL_H

#include "pictureitem_interface.h"
#include "teximg.h"

#include <QGLWidget>
#include <QFutureWatcher>
#include <QtConcurrentMap>

class PictureItemGL : public QGLWidget, public PictureItemInterface
{
    Q_OBJECT

public:
    explicit PictureItemGL(PictureItemData *data, QWidget *parent);
    ~PictureItemGL();
    void setRotation(const qreal current, const qreal previous);
    void setBackgroundColor(const QColor &color);
    void setImage(const QImage &img);
    void setNullImage();
    void setTexture(const QImage &tex, const int num);
    void setZoom(const qreal current, const qreal previous);
    QWidget *getWidget();

private:
    void loadTextures(QList<TexIndex *> indexes);
    void updateSize();
    void clearTextures();

    int m_returnTexCount;
    QFutureWatcher< QImage > *m_loader_texture;
    qreal m_scaleX;
    qreal m_scaleY;
    QVector < QVector <GLuint> > m_textures;
    TexImg *m_texImg;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

private slots:
    void textureFinished(int num);
};

inline void PictureItemGL::setZoom(const qreal, const qreal)
{ setRotation(m_data->getRotation(), m_data->getRotation()); }

#endif // PICTUREITEM_GL_H
#endif // QT5
