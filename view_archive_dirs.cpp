#include "view_archive_dirs.h"
#include "helper.h"
#include <QDebug>
#include <QDateTime>
ViewArchiveDirs::ViewArchiveDirs(QWidget *parent)
    : QTreeView(parent)
    , m_proxy(new MySortFilterProxyModel())
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setUniformRowHeights(true);
    this->setHeaderHidden(true);
}

void ViewArchiveDirs::setModel(QAbstractItemModel *model)
{
    m_proxy->setSourceModel(model);
    QTreeView::setModel(m_proxy);
}

void ViewArchiveDirs::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    emit currentRowChanged(m_proxy->mapToSource(current));
}

void ViewArchiveDirs::setCurrentIndexFromSource(const QModelIndex &index)
{
    QTreeView::setCurrentIndex(m_proxy->mapFromSource(index));

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
    return !(index0.data(Helper::ROLE_TYPE).toInt() == Helper::TYPE_ARCHIVE_FILE);
}

bool ViewArchiveDirs::MySortFilterProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    return (source_column == 0);
}
