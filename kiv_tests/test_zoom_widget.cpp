#include "test_zoom_widget.h"
#include <QtTest>

void TestZoomWidget::init()
{
    m_defaultZoomValues = QVector<qreal>({0.5, 1, 2, 3, 4});
    m_zoomWidget = new ZoomWidget(m_defaultZoomValues);
}

void TestZoomWidget::cleanup()
{
    delete m_zoomWidget;
}

void TestZoomWidget::testZoomInLessThanDefaultRange()
{
    // Default zoom in step is 25%
    m_zoomWidget->setZoom(0.1);
    m_zoomWidget->zoomIn();
    QCOMPARE(m_zoomWidget->getZoom(), 0.125);  // 0.1 * 1.25
    m_zoomWidget->zoomIn();
    QCOMPARE(m_zoomWidget->getZoom(), 0.15625); // 0.125 * 1.25
}

void TestZoomWidget::testZoomInGreaterThanDefaultRange()
{
    // Default zoom in step is 25%
    m_zoomWidget->setZoom(5);
    m_zoomWidget->zoomIn();
    QCOMPARE(m_zoomWidget->getZoom(), 6.25);  // 5 * 1.25
    m_zoomWidget->zoomIn();
    QCOMPARE(m_zoomWidget->getZoom(), 7.8125); // 6.25 * 1.25
}

void TestZoomWidget::testZoomInInsideDefaultRange()
{
    m_zoomWidget->setZoom(1);
    m_zoomWidget->zoomIn();
    QCOMPARE(m_zoomWidget->getZoom(), 2.0);
    m_zoomWidget->zoomIn();
    QCOMPARE(m_zoomWidget->getZoom(), 3.0);
    m_zoomWidget->zoomIn();
    QCOMPARE(m_zoomWidget->getZoom(), 4.0);
}

void TestZoomWidget::testZoomInToDefaultRange()
{
    // Default zoom in step is 25%
    m_zoomWidget->setZoom(0.45);
    m_zoomWidget->zoomIn();
    // 0.45 * 1.25 = 0.5625 but 0.5 is first default
    QCOMPARE(m_zoomWidget->getZoom(), 0.5);
    m_zoomWidget->zoomIn();
    QCOMPARE(m_zoomWidget->getZoom(), 1.0);
}

void TestZoomWidget::testZoomOutGreaterThanDefaultRange()
{
    // Default zoom out step is 25%
    m_zoomWidget->setZoom(15);
    m_zoomWidget->zoomOut();
    QCOMPARE(m_zoomWidget->getZoom(), 12.0);  // 15.0 / 1.25
    m_zoomWidget->zoomOut();
    QCOMPARE(m_zoomWidget->getZoom(), 9.6);  // 12.0 / 1.25
}

void TestZoomWidget::testZoomOutLessThanDefaultRange()
{
    // Default zoom out step is 25%
    m_zoomWidget->setZoom(0.1);
    m_zoomWidget->zoomOut();
    QCOMPARE(m_zoomWidget->getZoom(), 0.08);  // 0.1 / 1.25
    m_zoomWidget->zoomOut();
    QCOMPARE(m_zoomWidget->getZoom(), 0.064);  // 0.08 / 1.25
}

void TestZoomWidget::testZoomOutInsideDefaultRange()
{
    m_zoomWidget->setZoom(3);
    m_zoomWidget->zoomOut();
    QCOMPARE(m_zoomWidget->getZoom(), 2.0);
    m_zoomWidget->zoomOut();
    QCOMPARE(m_zoomWidget->getZoom(), 1.0);
    m_zoomWidget->zoomOut();
    QCOMPARE(m_zoomWidget->getZoom(), 0.5);
}

void TestZoomWidget::testZoomOutToDefaultRange()
{
    // Default zoom in step is 25%
    m_zoomWidget->setZoom(4.5);
    m_zoomWidget->zoomOut();
    // 4.5 / 1.25 = 3.6 but 4.0 is biggest default
    QCOMPARE(m_zoomWidget->getZoom(), 4.0);
    m_zoomWidget->zoomOut();
    QCOMPARE(m_zoomWidget->getZoom(), 3.0);
}
