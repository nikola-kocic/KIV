#ifndef PICTUREITEMGL_H
#define PICTUREITEMGL_H

#include "komicviewer_enums.h"
#include "pictureitem_shared.h"
#include "teximg.h"

#include <QtCore/qtimer.h>
#include <QtOpenGL/qgl.h>
#include <QtCore/qtconcurrentrun.h>
#include <QtCore/qfuturewatcher.h>
#include <QtCore/qtconcurrentmap.h>

#include <QtCore/QTime>

class PictureItemGL : public QGLWidget
{
    Q_OBJECT

public:
    PictureItemGL(PictureItemShared *picItemShared, QWidget *parent = 0, Qt::WindowFlags f = 0);
    void setRotation(qreal r);
    void setFile(const FileInfo &info);
    void setClearColor(const QColor &color);

private:
    void updateSize();
    PictureItemShared *picItemShared;

    qreal scaleX;
    qreal scaleY;
    int offsetX;
    int offsetY;
    QColor clearColor;
    QPoint lastPos;
    QVector < QVector <GLuint> > textures;
    TexImg *ti;
    FileInfo currentFileInfo;
    QTime t;
    QFutureWatcher< QImage > *textureLoader;
    int returnTexCount;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

signals:
    void zoomChanged();
    void imageChanged();

public slots:
    void setZoom(qreal current, qreal previous);

private slots:
    void textureFinished(int num);

};


#endif // PICTUREITEMGL_H
