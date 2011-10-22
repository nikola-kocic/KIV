#ifndef PICTUREITEMGL_H
#define PICTUREITEMGL_H

#include "komicviewer_enums.h"
#include "pictureitem_shared.h"
#include "teximg.h"

#include <QtCore/qtimer.h>
#include <QtOpenGL/qgl.h>

class PictureItemGL : public QGLWidget
{
    Q_OBJECT

public:
    PictureItemGL( PictureItemShared* pis, QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~PictureItemGL();
    void setRotation( qreal r );
    void setFile(const ZipInfo &info);
    void setClearColor( const QColor &color );

private:
    void updateSize();
    PictureItemShared* pis;

    qreal scaleX;
    qreal scaleY;
    int offsetX;
    int offsetY;
    QColor clearColor;
    QPoint lastPos;
    QVector< QVector <GLuint> > textures;
    TexImg *ti;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height );

signals:
    void zoomChanged();

public slots:
    void setZoom( qreal current, qreal previous );

private slots:
    void setPixmap();

};


#endif // PICTUREITEMGL_H
