#ifndef LOCATION_WIDGET_H
#define LOCATION_WIDGET_H

#include <QCompleter>
#include <QLineEdit>
#include <QUrl>

class UrlNavigator : public QLineEdit
{
    Q_OBJECT

public:
    explicit UrlNavigator(QAbstractItemModel *model,
                          const QUrl &url,
                          QWidget *parent = nullptr);
    int historyIndex() const;
    bool setHistoryIndex(int index);

    bool goBack();
    bool goForward();

    int historySize() const;
    QList<QUrl> getHistory() const;

public slots:
    void setLocationUrl(const QUrl &url);
    void focus();

signals:
    void urlChanged(const QUrl &url);
    void historyChanged();

protected:
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setLocationUrlInternal(const QUrl &url);
    void updateContent();
    QAbstractItemModel *m_model;
    QCompleter *m_completer;
    QUrl m_currentUrl;
    QList<QUrl> m_history;
    int m_historyIndex;
    const int m_historyMax;

private slots:
    void on_returnPressed();
};

#endif  // LOCATION_WIDGET_H
