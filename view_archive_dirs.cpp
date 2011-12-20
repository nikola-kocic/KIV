#include "view_archive_dirs.h"
#include "helper.h"
#include <QDebug>
#include <QDateTime>
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

void ViewArchiveDirs::setCurrentIndexFromSource(const QModelIndex &index)
{
    QTreeView::setCurrentIndex(this->proxy->mapFromSource(index));

    QModelIndex expandIndex = this->currentIndex();
    while (!this->isExpanded(expandIndex) && expandIndex.isValid())
    {
        this->expand(expandIndex);
        expandIndex = expandIndex.parent();
    }
}

bool ViewArchiveDirs::MySortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = this->sourceModel()->index(sourceRow, 0, sourceParent);
    return !(index0.data(ROLE_TYPE).toInt() == TYPE_ARCHIVE_FILE);
}

bool ViewArchiveDirs::MySortFilterProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    return (source_column == 0);
}
