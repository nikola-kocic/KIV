#ifndef FILEVIEWWITHPROXY_H
#define FILEVIEWWITHPROXY_H

#include <memory>

#include <QAbstractItemView>
#include <QAbstractProxyModel>
#include <QModelIndex>
#include "kiv/src/widgets/files_view/ifileview.h"


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

public slots:
    void setCurrentIndex(const QModelIndex &index);

private:

protected:
    std::unique_ptr<QAbstractItemView> m_view;
    std::unique_ptr<QAbstractProxyModel> m_proxy;

protected slots:
    void currentChanged(
            const QModelIndex &current,
            const QModelIndex &previous
            );
};


#endif // FILEVIEWWITHPROXY_H
