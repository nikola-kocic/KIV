#ifndef IFILEVIEW_H
#define IFILEVIEW_H

#include <QObject>
#include <QAbstractItemModel>
#include <QWidget>

class IFileView : public QObject
{
    Q_OBJECT
public:
    explicit IFileView(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IFileView() {}
    virtual QWidget *getWidget() = 0;
    virtual void setModel(QAbstractItemModel *model) = 0;
    virtual QModelIndex currentIndex () const = 0;
    virtual void setCurrentIndex(const QModelIndex &index) = 0;

signals:
    void currentRowChanged(const QModelIndex &);
};

#endif // IFILEVIEW_H
