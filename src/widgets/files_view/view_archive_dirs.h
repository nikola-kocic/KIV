#ifndef VIEWARCHIVEDIRS_H
#define VIEWARCHIVEDIRS_H

#include <memory>
#include <QTreeView>
#include <QSortFilterProxyModel>

#include "models/modelwrapper.h"
#include "widgets/files_view/fileviewwithproxy.h"


class ViewArchiveDirs : public FileViewWithProxy
{
    Q_OBJECT
public:
    explicit ViewArchiveDirs(
            std::unique_ptr<QTreeView> view,
            std::unique_ptr<QAbstractProxyModel> proxy,
            QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model) override;
    void setCurrentIndex(const QModelIndex &index) override;
};

#endif // VIEWARCHIVEDIRS_H
