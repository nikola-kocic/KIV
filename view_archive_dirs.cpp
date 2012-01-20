#include "view_archive_dirs.h"
#include "helper.h"

#include <QHeaderView>

ViewArchiveDirs::ViewArchiveDirs(QWidget *parent)
    : QTreeView(parent)
    , m_proxy(new MySortFilterProxyModel(this))
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setUniformRowHeights(true);
    this->header()->setResizeMode(QHeaderView::Stretch);
    this->setHeaderHidden(true);
}

void ViewArchiveDirs::setModel(QAbstractItemModel *model)
{
    m_proxy->setSourceModel(model);
    QTreeView::setModel(m_proxy);

    for (int i = 1; i < this->header()->count(); ++i)
    {
        this->hideColumn(i);
    }
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

ViewArchiveDirs::MySortFilterProxyModel::MySortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}
