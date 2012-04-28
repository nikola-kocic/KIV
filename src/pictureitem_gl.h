#ifndef PICTUREITEM_GL_H
#define PICTUREITEM_GL_H

#include "pictureitem.h"
#include "teximg.h"

#include <QGLWidget>
#include <QFutureWatcher>

class PictureItem;

class PictureItemGL : public QGLWidget
{
    Q_OBJECT

public:
    explicit PictureItemGL(PictureItem* parent, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0);
    ~PictureItemGL();
    void setRotation(const qreal current, const qreal previous);
    void updateClearColor();
    void setImage(const QImage &img);
    void setTexture(const QImage &tex, const int num);

private:
    void loadTextures(QList<TexIndex *> indexes);
    void updateSize();
    void clearTextures();
    PictureItem *m_picItem;

    int m_returnTexCount;
    QFutureWatcher< QImage > *m_loader_texture;
    qreal m_scaleX;
    qreal m_scaleY;
    QVector < QVector <GLuint> > m_textures;
    QVector < QVector <GLuint> > m_old_textures;
    TexImg *m_texImg;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

private slots:
    void textureFinished(int num);
};

#endif // PICTUREITEM_GL_H
