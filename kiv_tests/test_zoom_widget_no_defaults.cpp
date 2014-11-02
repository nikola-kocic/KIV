#include "test_zoom_widget_no_defaults.h"
#include <QtTest>

void TestZoomWidgetNoDefaults::init()
{
    m_defaultZoomValues = QVector<qreal>(0);
    m_zoomWidget = new ZoomWidget(m_defaultZoomValues);
}

void TestZoomWidgetNoDefaults::cleanup()
{
    delete m_zoomWidget;
}

void TestZoomWidgetNoDefaults::testZoomIn()
{
    // Default zoom in step is 25%
    m_zoomWidget->setZoom(5);
    m_zoomWidget->zoomIn();
    QCOMPARE(m_zoomWidget->getZoom(), 6.25);  // 5 * 1.25
    m_zoomWidget->zoomIn();
    QCOMPARE(m_zoomWidget->getZoom(), 7.8125); // 6.25 * 1.25
}


void TestZoomWidgetNoDefaults::testZoomOut()
{
    // Default zoom out step is 25%
    m_zoomWidget->setZoom(15);
    m_zoomWidget->zoomOut();
    QCOMPARE(m_zoomWidget->getZoom(), 12.0);  // 15.0 / 1.25
    m_zoomWidget->zoomOut();
    QCOMPARE(m_zoomWidget->getZoom(), 9.6);  // 12.0 / 1.25
}
