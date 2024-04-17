#ifndef ZOOMWIDGET_H
#define ZOOMWIDGET_H

#include <QComboBox>

#include <vector>

class ZoomWidget : public QComboBox
{
    Q_OBJECT
public:
    explicit ZoomWidget(std::vector<double> defaultZoomSizes, QWidget *parent = nullptr);
    double getZoom();
    void on_zoomChanged(double current, double previous);
    void zoomIn();
    void zoomOut();
    void setZoom(double value);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    void on_textChanged();
    void on_activated(int index);

    double m_zoomValue;
    const std::vector<double> m_defaultZoomSizes;

signals:
    void zoomChanged(double value);
};

#endif // ZOOMWIDGET_H
