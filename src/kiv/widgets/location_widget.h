#ifndef LOCATION_WIDGET_H
#define LOCATION_WIDGET_H

#include <QCompleter>
#include <QLineEdit>
#include <QUrl>

class LocationWidget : public QLineEdit
{
    Q_OBJECT

public:
    explicit LocationWidget(QAbstractItemModel *model, const QUrl &url, QWidget *parent = 0);

public slots:
    void setLocationUrl(const QUrl &url);

signals:
    void urlChanged(const QUrl &url);

protected:
    void focusOutEvent(QFocusEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void setLocationUrlInternal(const QUrl &url);
    QAbstractItemModel *m_model;
    QCompleter *m_completer;
    QUrl m_currentUrl;

private slots:
    void on_returnPressed();
};

#endif // LOCATION_WIDGET_H
