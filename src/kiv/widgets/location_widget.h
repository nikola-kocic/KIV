#ifndef LOCATION_WIDGET_H
#define LOCATION_WIDGET_H

#include <QCompleter>
#include <QLineEdit>

#include "fileinfo.h"

class LocationWidget : public QLineEdit
{
    Q_OBJECT
public:
    explicit LocationWidget(QAbstractItemModel *model, QWidget *parent = 0);


public slots:
    void setText(const QString &text);
    void setFileInfo(const FileInfo &fileinfo);

protected:
    void focusOutEvent(QFocusEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    QAbstractItemModel *m_model;
    QCompleter *m_completer;
    FileInfo m_current_fileinfo;

private slots:
    void on_returnPressed();

signals:
    void locationChanged(const FileInfo &fileinfo);
};

#endif // LOCATION_WIDGET_H
