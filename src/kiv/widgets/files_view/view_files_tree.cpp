#include "widgets/files_view/view_files.h"

#include <QHeaderView>

TreeViewFiles::TreeViewFiles(QWidget *parent)
    : QTreeView(parent)
{
    this->setUniformRowHeights(true);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setRootIsDecorated(false);
    this->setItemsExpandable(false);
}

void TreeViewFiles::rowsInserted(const QModelIndex &parent, int start, int end)
{
    QTreeView::rowsInserted(parent, start, end);
    QModelIndexList indexes;
    for (int i = start; i <= end; ++i)
    {
        if (parent.child(i, 0).isValid())
        {
            indexes.append(parent.child(i, 0));
        }

//        DEBUGOUT << parent << i << parent.child(i, 0)
//                 << parent.child(i, 0).data().toString();
    }
    emit rowsInserted(indexes);
}
