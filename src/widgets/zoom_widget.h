#ifndef ZOOMWIDGET_H
#define ZOOMWIDGET_H

#include <QComboBox>
#include <QVector>

class ZoomWidget : public QComboBox
{
    Q_OBJECT
public:
    explicit ZoomWidget(const QVector<qreal> defaultZoomSizes,
                        QWidget *parent = nullptr);
    qreal getZoom();
    void on_zoomChanged(qreal current, qreal previous);
    void zoomIn();
    void zoomOut();
    void setZoom(qreal value);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    void on_textChanged();
    void on_activated(int index);

    qreal m_zoomValue;
    const QVector<qreal> m_defaultZoomSizes;

signals:
    void zoomChanged(qreal value);
};

#endif  // ZOOMWIDGET_H
