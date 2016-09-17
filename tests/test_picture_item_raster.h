#ifndef TESTPICTUREITEMRASTER_H
#define TESTPICTUREITEMRASTER_H

#include "widgets/picture_item/pictureitem_data.h"
#include "widgets/picture_item/pictureitem_raster.h"

#include <QObject>

class TestPictureItemRaster : public QObject
{
    Q_OBJECT

public:
    TestPictureItemRaster(QObject *parent = 0);

private:
    PictureItemData m_data;
    PictureItemRaster *m_pictureitem;
    QSize m_viewSize;
    QRect m_viewRect;
    QImage m_displayedImage;

private Q_SLOTS:
    void init();
    void cleanup();
    void testRenderNoImage();
    void testRenderNormal();
    void testRenderZoomed();
    void testRenderZoomedCropped();
};

#endif // TESTPICTUREITEMRASTER_H
