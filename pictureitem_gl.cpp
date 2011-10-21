#include "pictureitem_gl.h"
#include "settings.h"

#include <QtCore/qdebug.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtGui/qevent.h>

PictureItemGL::PictureItemGL( PictureItemShared* pis, QWidget * parent, Qt::WindowFlags f )
{
    this->pis = pis;
    connect( pis, SIGNAL( pixmapChanged() ), this, SLOT( setPixmap() ) );
    connect( pis, SIGNAL( zoomChanged(qreal,qreal) ), this, SLOT( setZoom( qreal,qreal ) ) );
    connect( pis, SIGNAL( update() ), this, SLOT( update() ) );

    ti = 0;
    clearColor = Qt::black;
    offsetX = offsetY = dimY = dimX = 0;
}

void PictureItemGL::setTextures(TexImg *t)
{
    if(ti == NULL)
    {
        ti = t;
    }
    else
    {
        for ( int hIndex = 0; hIndex < texObjArrayTile.count(); ++hIndex )
        {
            texObjArrayTile[hIndex].resize( ti->vTile->tileCount );

            for(int vIndex=0; vIndex<texObjArrayTile.at(hIndex).count(); vIndex++)
            {
                glDeleteTextures(1, &texObjArrayTile.at(hIndex).at(vIndex));
            }
        }
    }


    qDebug() << "deleted";
    this->texObjArrayTile = QVector < QVector < GLuint > >( ti->hTile->tileCount );
    for ( int hIndex = 0; hIndex < ti->hTile->tileCount; ++hIndex )
    {
        texObjArrayTile[hIndex].resize( ti->vTile->tileCount );

        for(int vIndex=0; vIndex<ti->vTile->tileCount; vIndex++)
        {
            glGenTextures(1, &texObjArrayTile[hIndex][vIndex]);
            glBindTexture(GL_TEXTURE_2D, texObjArrayTile[hIndex][vIndex]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ti->hTile->tileSize.at(hIndex), ti->vTile->tileSize.at(vIndex), 0, GL_RGBA, GL_UNSIGNED_BYTE, ti->pow2TileBuffer.at(hIndex).at(vIndex));
        }
    }


    updateGL();
}

void PictureItemGL::setPixmap()
{
//    deleteTexture( texture );
    if( pis->getPixmap().isNull() )
    {
//        texture = 0;
        return;
    }

//    texture = bindTexture( pis->getPixmap(), GL_TEXTURE_2D, GL_RGBA, QGLContext::LinearFilteringBindOption );

    dimX = (double)pis->getPixmapSize().width() / 2;
    dimY = (double)pis->getPixmapSize().height() / 2;

    setRotation( 0 );
    if( pis->getLockMode() != LockMode::Zoom )
    {
        pis->setZoom( 1 );
    }

    pis->boundingRect = QRect( 0, 0, ( pis->getPixmapSize().width() * pis->getZoom() ), ( pis->getPixmapSize().height() * pis->getZoom() ) );

    pis->afterPixmapLoad();

    updateSize();
    initializeGL();
    updateGL();
}

PictureItemGL::~PictureItemGL()
{
}

void PictureItemGL::setClearColor( const QColor &color )
{
    clearColor = color;
    updateGL();
}

void PictureItemGL::initializeGL()
{
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glEnable( GL_TEXTURE_2D );
//    if( texture != 0 )
//    {
//        glBindTexture ( GL_TEXTURE_2D, texture );
//    }
}

void PictureItemGL::updateSize()
{
    offsetX = ( pis->boundingRect.width() - this->width() ) / 2;
    offsetY = ( pis->boundingRect.height() - this->height() ) / 2;

    dimX =  pis->getPixmapSize().width() * ( pis->getZoom() / 2 );
    dimY =  pis->getPixmapSize().height() * ( pis->getZoom() / 2 );
}

void PictureItemGL::paintGL()
{
    qglClearColor( clearColor );
    glClear( GL_COLOR_BUFFER_BIT );

//    if( texture == 0 )
    if(ti == NULL)
    {
        return;
    }

    glMatrixMode( GL_TEXTURE_MATRIX );
    glLoadIdentity();

    glTranslated( ( pis->boundingRect.x() + ( offsetX > 0 ? offsetX : 0 ) ), ( pis->boundingRect.y() + ( offsetY > 0 ? offsetY : 0 ) ), 0 );
    glRotated( pis->getRotation(), 0 , 0, 1 );


    int oldQuadBottom = 0;
    for(int vIndex=0; vIndex<ti->vTile->tileCount; vIndex++)
    {
        int oldQuadRight = 0;
        int quadHeight = (ti->vTile->tileSize.at(vIndex)) / 2;


        for(int hIndex=0; hIndex<ti->hTile->tileCount; hIndex++)
        {
            int quadWidth = (ti->hTile->tileSize.at(hIndex)) / 2;

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// GL_NEAREST is another choice
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, texObjArrayTile.at(hIndex).at(vIndex));
            glBegin(GL_QUADS);

//            qDebug() << quadWidth << quadHeight;

            glTexCoord2f( 0.0f, 1.0f ); /* lower left corner of image */
            glVertex2f( -quadWidth, quadHeight + (oldQuadBottom / 2) );

            glTexCoord2f( 1.0f, 1.0f ); /* lower right corner of image */
            glVertex2f( quadWidth,  quadHeight + (oldQuadBottom / 2) );

            glTexCoord2f( 1.0f, 0.0f ); /* upper right corner of image */
            glVertex2f( quadWidth, -quadHeight + (oldQuadBottom / 2) );

            glTexCoord2f( 0.0f, 0.0f ); /* upper left corner of image */
            glVertex2f( -quadWidth, -quadHeight + (oldQuadBottom / 1.5) );
            glEnd();

            oldQuadRight += ti->hTile->tileSize.at(hIndex);
        }
        oldQuadBottom += ti->vTile->tileSize.at(vIndex);
    }

//    qDebug() << "***";

}

void PictureItemGL::resizeGL( int width, int height )
{
    updateSize();
    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( ( -width / 2), ( +width / 2 ), ( +height / 2 ), ( -height / 2 ), 0, 1 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}


//Region Rotation

void PictureItemGL::setRotation( qreal r )
{
//    if( texture == 0 ) return;

    pis->setRotation(r);

    QTransform tRot;
    tRot.translate( pis->boundingRect.x(), pis->boundingRect.y() );
    tRot.scale( pis->getZoom(), pis->getZoom() );
    tRot.translate( ( pis->getPixmapSize().height() / 2 ), ( pis->getPixmapSize().width() / 2 ) );
    tRot.rotate( pis->getRotation() );
    tRot.translate( ( -pis->getPixmapSize().height() / 2 ), ( -pis->getPixmapSize().width() / 2 ) );
    QRect transformedRot = tRot.mapRect( pis->getPixmap().rect() );

    pis->boundingRect.setWidth( transformedRot.width() );
    pis->boundingRect.setHeight( transformedRot.height() );

    if( ( pis->boundingRect.height() + pis->boundingRect.y() ) < this->height() )
    {
        pis->boundingRect.translate(0, ( this->height() - (pis->boundingRect.height() + pis->boundingRect.y() ) ) );
    }

    if( pis->boundingRect.height()  < this->height() )
    {
        pis->boundingRect.moveTop( 0 );
    }

    updateSize();
    updateGL();
}


//End Region Rotation

//Region Zoom

void PictureItemGL::setZoom( qreal current, qreal previous )
{
//    if( texture == 0 ) return;

    qreal scaledW = ( pis->boundingRect.width() / previous ) * current;
    qreal scaledH = ( pis->boundingRect.height() / previous ) * current;
    QPointF p = pis->pointToOrigin( scaledW, scaledH );
    pis->boundingRect = QRectF(p.x(), p.y(), scaledW, scaledH );

    setRotation( pis->getRotation() );
    pis->avoidOutOfScreen();

    updateGL();

    emit zoomChanged();
}
