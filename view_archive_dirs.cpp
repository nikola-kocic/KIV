#include "view_archive_dirs.h"
#include "komicviewer_enums.h"
#include <QDebug>

ViewArchiveDirs::ViewArchiveDirs()
{
    proxy = new MySortFilterProxyModel();
}


void ViewArchiveDirs::setModel ( QAbstractItemModel * model )
{
    proxy->setSourceModel(model);
    QTreeView::setModel(proxy);
}

bool ViewArchiveDirs::MySortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    return !(index0.data(ROLE_TYPE).toInt() > makeArchiveNumberForItem(0));
}
