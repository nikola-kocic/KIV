#include "view_files.h"
#include <QPainter>
ViewFiles::ListViewFiles::ListViewFiles(ViewFiles *parent)
{
    m_parent = parent;
    m_proxy = new QSortFilterProxyModel(this);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setUniformItemSizes(true);
    this->setViewMode(QListView::IconMode);
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

void ViewFiles::ListViewFiles::startShowingThumbnails()
{
    for (int i = 0; i < m_row_delegates.size(); ++i)
    {
        this->setItemDelegateForRow(i , 0);
        m_row_delegates.at(i)->deleteLater();
    }
    m_row_delegates.clear();

    if (m_proxy->rowCount(this->rootIndex()) == 0)
    {
        return;
    }

    //add item delegate
    for (int i = 0; i < m_proxy->rowCount(this->rootIndex()); ++i)
    {
        FileInfo pli_info;

        QModelIndex index = m_proxy->index(i, 0, this->rootIndex());
        int type = m_proxy->data(index, ROLE_TYPE).toInt();
        QString name = m_proxy->data(index, Qt::DisplayRole).toString();
        pli_info = m_parent->m_currentInfo;
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            if (!m_parent->m_currentInfo.isZip())
            {
                pli_info.imageFileName = name;
            }
            else
            {
                pli_info.zipImageFileName = name;
            }
        }
        else
        {
            pli_info.containerPath += "/" + name;
        }

        ThumbnailItemDelegate *tid = new ThumbnailItemDelegate(ThumbnailInfo(pli_info, m_parent->m_thumb_size), index, this);
        m_row_delegates.append(tid);
        connect(tid, SIGNAL(thumbnailFinished(QModelIndex)), m_parent, SLOT(on_thumbnail_finished(QModelIndex)));
        this->setItemDelegateForRow(i, tid);
    }

}
