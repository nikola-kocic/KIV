#ifndef FILEVIEWWITHPROXY_H
#define FILEVIEWWITHPROXY_H

#include <memory>

#include <QAbstractItemView>
#include <QAbstractProxyModel>
#include <QModelIndex>
#include "widgets/files_view/ifileview.h"


class FileViewWithProxy : public IFileView
{
    Q_OBJECT
public:
    explicit FileViewWithProxy(
            std::unique_ptr<QAbstractItemView> view,
            std::unique_ptr<QAbstractProxyModel> proxy,
            QWidget *parent = nullptr);
    virtual QWidget *getWidget() override;
    void setModel(QAbstractItemModel *model) override;
    QModelIndex currentIndex() const override;
    void setCurrentIndex(const QModelIndex &index) override;

private:

protected:
    std::unique_ptr<QAbstractItemView> m_view;
    std::unique_ptr<QAbstractProxyModel> m_proxy;
    void currentChanged(
            const QModelIndex &current,
            const QModelIndex &previous
            );
};


#endif // FILEVIEWWITHPROXY_H
