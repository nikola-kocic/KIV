#ifndef LOCATION_WIDGET_H
#define LOCATION_WIDGET_H

#include <QCompleter>
#include <QLineEdit>
#include <QUrl>

class UrlNavigator : public QLineEdit
{
    Q_OBJECT

public:
    explicit UrlNavigator(QAbstractItemModel *model, const QUrl &url, QWidget *parent = 0);
    bool goBack();
    bool goForward();

public slots:
    void setLocationUrl(const QUrl &url);

signals:
    void urlChanged(const QUrl &url);

protected:
    void focusOutEvent(QFocusEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void setLocationUrlInternal(const QUrl &url);
    void updateContent();
    QAbstractItemModel *m_model;
    QCompleter *m_completer;
    QUrl m_currentUrl;
    QList<QUrl> m_history;
    int m_currentHistoryIndex;
    const int m_historyMax;

private slots:
    void on_returnPressed();
};

#endif // LOCATION_WIDGET_H