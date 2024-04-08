#include "test_picture_item_raster.h"
#include <QtTest>
#include <QPainter>

TestPictureItemRaster::TestPictureItemRaster(QObject *parent)
    : QObject(parent)
    , m_viewSize(QSize(100, 100))
    , m_viewRect(QRect(QPoint(0, 0), m_viewSize))
    , m_displayedImage(QImage(":/assets/chess-pattern.png"))
{
}

void TestPictureItemRaster::init()
{
    m_data = PictureItemData();
    m_data.m_color_clear = QColor::fromRgb(192, 192, 192);
    m_pictureitem = new PictureItemRaster(&m_data, ZoomFilter::Good, NULL);
}

void TestPictureItemRaster::cleanup()
{
    delete m_pictureitem;
}

void TestPictureItemRaster::compareToExpected(const QString& expectedPath) {
    QPixmap viewPixmap(m_viewSize);
    QPainter p(&viewPixmap);
    QPaintEvent e(m_viewRect);
    m_pictureitem->paint(p, &e);
    p.end();

    const auto actualImage = viewPixmap.toImage();
    const auto expectedImg = QImage(expectedPath).convertToFormat(actualImage.format());
    QCOMPARE(expectedImg, actualImage);
}

void TestPictureItemRaster::testRenderNoImage()
{
    QPixmap expectedPixmap(m_viewSize);
    QPainter pExpected(&expectedPixmap);
    pExpected.fillRect(m_viewRect, m_data.m_color_clear);
    pExpected.end();

    QPixmap viewPixmap(m_viewSize);
    QPainter p(&viewPixmap);
    QPaintEvent e(m_viewRect);
    m_pictureitem->paint(p, &e);
    p.end();
    QCOMPARE(expectedPixmap, viewPixmap);
}

void TestPictureItemRaster::testRenderNormal()
{
    m_pictureitem->setImage(m_displayedImage);
    m_data.setPixmapNull(false);
    m_data.setImageSize(m_displayedImage.size());
    m_data.setWidgetSize(m_viewSize);
    m_data.updateSize();

    compareToExpected(":/assets/expected-normal.png");
}

void TestPictureItemRaster::testRenderZoomed()
{
    m_pictureitem->setImage(m_displayedImage);
    m_data.setPixmapNull(false);
    m_data.setImageSize(m_displayedImage.size());
    m_data.setWidgetSize(m_viewSize);
    qreal out_zoom;
    m_data.setZoom(2.0, out_zoom);

    compareToExpected(":/assets/expected-zoomed.png");
}

void TestPictureItemRaster::testRenderZoomedCropped()
{
    m_pictureitem->setImage(m_displayedImage);
    m_data.setPixmapNull(false);
    m_data.setImageSize(m_displayedImage.size());
    m_data.setWidgetSize(m_viewSize);
    qreal out_zoom;
    m_data.setZoom(3.0, out_zoom);

    compareToExpected(":/assets/expected-zoomed-cropped.png");
}
