#include "test_pictureitem_data.h"
#include <QtTest>

void TestPictureItemData::init()
{
    p_data = PictureItemData();
    p_data.setImageSize(QSize(800, 800));
    p_data.setWidgetSize(QSize(1000, 1000));
    p_data.setPixmapNull(false);
    p_data.updateSize();
}

void TestPictureItemData::testZoomIn_FittedBefore_NotAfter()
{
    qreal zoom;
    p_data.setZoom(2.0, zoom);
    QCOMPARE(zoom, 2.0);
    QRectF bounding_rect = p_data.getBoundingRect();
    QCOMPARE(bounding_rect, QRectF(-300, -300, 1600, 1600));
    QPointF offset = p_data.getOffset();
    QCOMPARE(offset, QPointF(0, 0));
    QCOMPARE(p_data.getImageSizeOriginal(), QSizeF(800, 800));
    QCOMPARE(p_data.getImageSizeTransformed(), QSizeF(800, 800));
}

void TestPictureItemData::testZoomIn_FittedPerfectlyBefore()
{
    p_data.setImageSize(QSize(1000, 1000));
    p_data.updateSize();
    qreal zoom;
    p_data.setZoom(2.0, zoom);
    QCOMPARE(zoom, 2.0);
    QRectF bounding_rect = p_data.getBoundingRect();
    QCOMPARE(bounding_rect, QRectF(-500, -500, 2000, 2000));
    QPointF offset = p_data.getOffset();
    QCOMPARE(offset, QPointF(0, 0));
    QCOMPARE(p_data.getImageSizeOriginal(), QSizeF(1000, 1000));
    QCOMPARE(p_data.getImageSizeTransformed(), QSizeF(1000, 1000));
}

void TestPictureItemData::testZoomIn_DidntFitBefore()
{
    p_data.setImageSize(QSize(1500, 1500));
    p_data.updateSize();
    qreal zoom;
    p_data.setZoom(2.0, zoom);
    QCOMPARE(zoom, 2.0);
    QRectF bounding_rect = p_data.getBoundingRect();

    // Wasn't moved so it started on top-left
    const QRectF expectedBoundingRect = QRectF(-500, -500, 3000, 3000);
    QCOMPARE(bounding_rect, expectedBoundingRect);
    QPointF offset = p_data.getOffset();
    QCOMPARE(offset, QPointF(0, 0));
    QCOMPARE(p_data.getImageSizeOriginal(), QSizeF(1500, 1500));
    QCOMPARE(p_data.getImageSizeTransformed(), QSizeF(1500, 1500));
}

void TestPictureItemData::testZoomIn_FitsAfter()
{
    qreal zoom;
    p_data.setZoom(1.2, zoom);
    QCOMPARE(zoom, 1.2);
    QRectF bounding_rect = p_data.getBoundingRect();
    QCOMPARE(bounding_rect, QRectF(0, 0, 960, 960));
    QPointF offset = p_data.getOffset();
    QCOMPARE(offset, QPointF(20, 20));
    QCOMPARE(p_data.getImageSizeOriginal(), QSizeF(800, 800));
    QCOMPARE(p_data.getImageSizeTransformed(), QSizeF(800, 800));
}

void TestPictureItemData::testZoomIn_DidntFitBefore_WasMoved()
{
    p_data.setImageSize(QSize(1500, 1500));

    // Center image to viewport
    p_data.beginDrag(QPoint(0,0));
    p_data.drag(QPoint(-250, -250));
    p_data.updateSize();
    QRectF bounding_rect = p_data.getBoundingRect();
    QCOMPARE(bounding_rect, QRectF(-250, -250, 1500, 1500));

    qreal zoom;
    p_data.setZoom(2.0, zoom);
    QCOMPARE(zoom, 2.0);
    bounding_rect = p_data.getBoundingRect();
    // 3000 - (1000 * 2) = 1000
    QCOMPARE(bounding_rect, QRectF(-1000, -1000, 3000, 3000));
    QPointF offset = p_data.getOffset();
    QCOMPARE(offset, QPointF(0, 0));
    QCOMPARE(p_data.getImageSizeOriginal(), QSizeF(1500, 1500));
    QCOMPARE(p_data.getImageSizeTransformed(), QSizeF(1500, 1500));
}

void TestPictureItemData::testZoomOut()
{
    qreal zoom;
    p_data.setZoom(0.75, zoom);
    QCOMPARE(zoom, 0.75);
    QRectF bounding_rect = p_data.getBoundingRect();
    QCOMPARE(bounding_rect, QRectF(0, 0, 600, 600));
    QPointF offset = p_data.getOffset();
    QCOMPARE(offset, QPointF(200, 200));
    QCOMPARE(p_data.getImageSizeOriginal(), QSizeF(800, 800));
    QCOMPARE(p_data.getImageSizeTransformed(), QSizeF(800, 800));
}

void TestPictureItemData::testRotate()
{
    p_data.setRotation(45.0);
    QRectF bounding_rect = p_data.getBoundingRect();
    const qreal expectedSize = 800 * qSqrt(2);
    const qreal expectedCoordinates = (1000 - expectedSize) / 2;
    const QRectF expectedBoundingRect =  QRectF(
                expectedCoordinates,
                expectedCoordinates,
                expectedSize,
                expectedSize);
    QCOMPARE(bounding_rect, expectedBoundingRect);
    p_data.setRotation(0);
    bounding_rect = p_data.getBoundingRect();
    QCOMPARE(bounding_rect, QRectF(0, 0, 800, 800));
}

void TestPictureItemData::testZoomOutRotate()
{
    testZoomOut();
    p_data.setRotation(45.0);
    QRectF bounding_rect = p_data.getBoundingRect();
    const qreal expectedSize = 600 * qSqrt(2);

    // Rotated image can fully display in view area
    const qreal expectedCoordinates = 0;

    const QRectF expectedBoundingRect =  QRectF(
                expectedCoordinates,
                expectedCoordinates,
                expectedSize,
                expectedSize);
    QCOMPARE(bounding_rect, expectedBoundingRect);
    p_data.setRotation(0);
    bounding_rect = p_data.getBoundingRect();
    QCOMPARE(bounding_rect, QRectF(0, 0, 600, 600));
}
