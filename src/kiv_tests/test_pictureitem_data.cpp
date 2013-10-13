#include "test_pictureitem_data.h"
#include <QtTest>

void TestPictureItemData::init()
{
    p_data = PictureItemData();
    p_data.setImageSize(QSize(1000, 1000));
    p_data.setWidgetSize(QSize(1000, 1000));
    p_data.setPixmapNull(false);
}

void TestPictureItemData::testZoomIn()
{
    qreal zoom;
    p_data.setZoom(1.25, zoom);
    QCOMPARE(zoom, 1.25);
    QRectF bounding_rect = p_data.getBoundingRect();
    QCOMPARE(bounding_rect, QRectF(-125, -125, 1250, 1250));
    QPointF offset = p_data.getOffset();
    QCOMPARE(offset, QPointF(0, 0));
    QCOMPARE(p_data.getImageSizeOriginal(), QSizeF(1000, 1000));
    QCOMPARE(p_data.getImageSizeTransformed(), QSizeF(1000, 1000));
}

void TestPictureItemData::testZoomOut()
{
    qreal zoom;
    p_data.setZoom(0.75, zoom);
    QCOMPARE(zoom, 0.75);
    QRectF bounding_rect = p_data.getBoundingRect();
    QCOMPARE(bounding_rect, QRectF(0, 0, 750, 750));
    QPointF offset = p_data.getOffset();
    QCOMPARE(offset, QPointF(125, 125));
    QCOMPARE(p_data.getImageSizeOriginal(), QSizeF(1000, 1000));
    QCOMPARE(p_data.getImageSizeTransformed(), QSizeF(1000, 1000));
}

void TestPictureItemData::testRotate()
{
    p_data.setRotation(45.0);
    QRectF bounding_rect = p_data.getBoundingRect();
    qreal expectedSize = 1000 * qSqrt(2);
    qreal expectedCoordinates = (1000 - expectedSize) / 2;
    QCOMPARE(bounding_rect, QRectF(
                 expectedCoordinates,
                 expectedCoordinates,
                 expectedSize,
                 expectedSize));
    p_data.setRotation(0);
    bounding_rect = p_data.getBoundingRect();
    QCOMPARE(bounding_rect, QRectF(0, 0, 1000, 1000));
}

