#include "view_files.h"

ViewFiles::ListViewFiles::ListViewFiles(ViewFiles *parent)
    : QListView(parent)
{
    m_parent = parent;
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setUniformItemSizes(true);
}

void ViewFiles::ListViewFiles::setViewMode(int mode)
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
