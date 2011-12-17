#include "view_files.h"

ViewFiles::ListViewFiles::ListViewFiles(ViewFiles *parent)
    : QListView(parent)
{
    m_parent = parent;
    m_proxy = new QSortFilterProxyModel(this);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setUniformItemSizes(true);
    this->setViewMode(QListView::IconMode);

    //add item delegate
    m_thumbnail_delegate = new ThumbnailItemDelegate(m_parent->m_thumb_size, this);
    connect(m_thumbnail_delegate, SIGNAL(thumbnailFinished(QModelIndex)), m_parent, SLOT(on_thumbnail_finished(QModelIndex)));
}

QModelIndex ViewFiles::ListViewFiles::getIndexFromProxy(const QModelIndex &index) const
{
    return m_proxy->mapToSource(index);
}

void ViewFiles::ListViewFiles::setModel(QAbstractItemModel *model)
{
    m_proxy->setSourceModel(model);
    QListView::setModel(m_proxy);
}

void ViewFiles::ListViewFiles::setCurrentIndex(const QModelIndex &index)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::setCurrentIndexFromSource" << index.data();
#endif
    this->setCurrentIndex(m_proxy->mapFromSource(index));
}

void ViewFiles::ListViewFiles::setRootIndex(const QModelIndex &index)
{
    QListView::setRootIndex(m_proxy->mapFromSource(index));
}

void ViewFiles::ListViewFiles::setShowThumbnails(bool b)
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

void ViewFiles::ListViewFiles::startShowingThumbnails()
{
    if (m_proxy->rowCount(this->rootIndex()) == 0)
    {
        return;
    }

    m_thumbnail_delegate->updateThumbnails(ThumbnailInfo(m_parent->m_currentInfo, m_parent->m_thumb_size));

}
