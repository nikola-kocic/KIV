#ifndef TEST_ZOOM_WIDGET_NO_DEFAULTS_H
#define TEST_ZOOM_WIDGET_NO_DEFAULTS_H

#include <QObject>

#include "kiv/src/widgets/zoom_widget.h"

class TestZoomWidgetNoDefaults: public QObject
{
    Q_OBJECT

private:
    ZoomWidget *m_zoomWidget;
    QVector<qreal> m_defaultZoomValues;

private Q_SLOTS:
    void init();
    void cleanup();
    void testZoomIn();
    void testZoomOut();
};

#endif // TEST_ZOOM_WIDGET_NO_DEFAULTS_H
