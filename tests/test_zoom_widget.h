#ifndef TEST_ZOOM_WIDGET_H
#define TEST_ZOOM_WIDGET_H

#include <QObject>

#include "widgets/zoom_widget.h"

class TestZoomWidget : public QObject
{
    Q_OBJECT

private:
    ZoomWidget m_zoomWidget{{0.5, 1, 2, 3, 4}};

private Q_SLOTS:
    void testZoomInLessThanDefaultRange();
    void testZoomInGreaterThanDefaultRange();
    void testZoomInInsideDefaultRange();
    void testZoomInToDefaultRange();
    void testZoomOutGreaterThanDefaultRange();
    void testZoomOutLessThanDefaultRange();
    void testZoomOutInsideDefaultRange();
    void testZoomOutToDefaultRange();
};

#endif // TEST_ZOOM_WIDGET_H
