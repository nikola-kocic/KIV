#include "widgets/files_view/view_files_list.h"

ListViewFiles::ListViewFiles(QWidget *parent)
    : QListView(parent)
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setUniformItemSizes(true);
}

void ListViewFiles::setViewMode(const FileViewMode mode)
{
    if (mode == FileViewMode::Icons)
    {
        QListView::setViewMode(QListView::IconMode);
    }
    else if (mode == FileViewMode::List)
    {
        QListView::setViewMode(QListView::ListMode);
    }
}

void ListViewFiles::rowsInserted(const QModelIndex &parent, int start, int end)
{
    QListView::rowsInserted(parent, start, end);
    QModelIndexList indexes;
    for (int i = start; i <= end; ++i)
    {
        QModelIndex currentIndex = model()->index(i, 0, parent);
        if (currentIndex.isValid())
        {
            indexes.append(currentIndex);
        }

//        DEBUGOUT << parent << i << parent.child(i, 0)
//                 << parent.child(i, 0).data().toString();
    }
    emit rowsInsertedSignal(indexes);
}
