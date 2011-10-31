#include "view_archive_dirs.h"
#include "helper.h"

ViewArchiveDirs::ViewArchiveDirs()
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setUniformRowHeights(true);
    this->setHeaderHidden(true);
    this->proxy = new MySortFilterProxyModel();
}

void ViewArchiveDirs::setModel(QAbstractItemModel *model)
{
    this->proxy->setSourceModel(model);
    QTreeView::setModel(this->proxy);
}

void ViewArchiveDirs::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    emit currentRowChanged(this->proxy->mapToSource(current));
}

void ViewArchiveDirs::show()
{
    this->setCurrentIndex(this->proxy->index(0,0,this->rootIndex()));
    this->expand(this->currentIndex());
    QTreeView::show();
}

void ViewArchiveDirs::setCurrentIndexFromSource(const QModelIndex &index)
{
    QTreeView::setCurrentIndex(this->proxy->mapFromSource(index));
    this->expand(this->currentIndex());
}

bool ViewArchiveDirs::MySortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = this->sourceModel()->index(sourceRow, 0, sourceParent);
    return !(index0.data(ROLE_TYPE).toInt() == TYPE_ARCHIVE_FILE);
}
