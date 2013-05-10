#ifndef ZOOMWIDGET_H
#define ZOOMWIDGET_H

#include <QComboBox>

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

public slots:
    void on_zoomChanged(qreal current, qreal previous);

private slots:
    void on_textChanged();

signals:
    void zoomChanged(qreal value);
};

#endif // ZOOMWIDGET_H
