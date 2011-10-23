#include "view_archive_dirs.h"
#include "komicviewer_enums.h"

ViewArchiveDirs::ViewArchiveDirs()
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    proxy = new MySortFilterProxyModel();
}

void ViewArchiveDirs::setModel(QAbstractItemModel * model)
{
    proxy->setSourceModel(model);
    QTreeView::setModel(proxy);
}

void ViewArchiveDirs::currentChanged(const QModelIndex & current, const QModelIndex & previous)
{
    QTreeView::currentChanged(current, previous);

    emit currentRowChanged(proxy->mapToSource(current));
}

void ViewArchiveDirs::show()
{
    this->setCurrentIndex(proxy->index(0,0,this->rootIndex()));
    this->expand(this->currentIndex());
    QTreeView::show();
}

void ViewArchiveDirs::setCurrentIndexFromSource(const QModelIndex &index)
{
    QTreeView::setCurrentIndex(proxy->mapFromSource(index));
    this->expand(this->currentIndex());
}

bool ViewArchiveDirs::MySortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    return !(index0.data(ROLE_TYPE).toInt() == TYPE_ARCHIVE_FILE);
}
