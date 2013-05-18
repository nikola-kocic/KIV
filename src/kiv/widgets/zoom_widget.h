#ifndef ZOOMWIDGET_H
#define ZOOMWIDGET_H

#include <QComboBox>
#include <QVector>

class ZoomWidget : public QComboBox
{
    Q_OBJECT
public:
    explicit ZoomWidget(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *event);
    void focusOutEvent(QFocusEvent *event);

private:
    qreal m_zoomValue;
    const QVector<qreal> m_defaultZoomSizes;

public slots:
    void on_zoomChanged(qreal current, qreal previous);
    void zoomIn();
    void zoomOut();
    void setZoom(qreal value);

private slots:
    void on_textChanged();
    void on_activated(int index);

signals:
    void zoomChanged(qreal value);
};

#endif // ZOOMWIDGET_H
