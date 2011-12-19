#include "view_files.h"

ListViewFiles::ListViewFiles(QWidget *parent)
    : QListView(parent)
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setUniformItemSizes(true);
}

void ListViewFiles::setViewMode(int mode)
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
        indexes.append(parent.child(i, 0));

//        qDebug() << "ListViewFiles::rowsInserted" << parent << i << parent.child(i, 0) << parent.child(i, 0).data().toString();
    }
    emit rowsInserted(indexes);
}
