#include "view_files.h"
#include <QHeaderView>

ViewFiles::TreeViewFiles::TreeViewFiles(ViewFiles *parent)
    : QTreeView(parent)
{
    m_parent = parent;


    this->setUniformRowHeights(true);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setRootIsDecorated(false);
    this->header()->setDefaultSectionSize(100);

    //add item delegate
    m_thumbnail_delegate = new ThumbnailItemDelegate(m_parent->m_thumb_size, this);
    connect(m_thumbnail_delegate, SIGNAL(thumbnailFinished(QModelIndex)), m_parent, SLOT(on_thumbnail_finished(QModelIndex)));
}

void ViewFiles::TreeViewFiles::startShowingThumbnails()
{
    if (this->model()->rowCount(this->rootIndex()) == 0)
    {
        return;
    }

    m_thumbnail_delegate->updateThumbnails(ThumbnailInfo(m_parent->m_currentInfo, m_parent->m_thumb_size));
}

void ViewFiles::TreeViewFiles::setShowThumbnails(bool b)
{
    if (b)
    {
        this->setItemDelegateForColumn(0, m_thumbnail_delegate);
        this->startShowingThumbnails();
    }
    else
    {
        this->setItemDelegateForColumn(0, 0);
    }
    this->doItemsLayout();
}
