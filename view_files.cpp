#include "view_files.h"
#include "settings.h"

#include <QFileIconProvider>

//#define DEBUG_VIEW_FILES

#ifdef DEBUG_VIEW_FILES
#include <QDebug>
#include <QTime>
#endif

ViewFiles::ViewFiles(QWidget *parent)
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_proxy = new QSortFilterProxyModel(this);
    m_thumb_size = QSize(200, 200);

    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setUniformItemSizes(true);

    m_returnThumbCount = 0;
    m_watcherThumbnail = new QFutureWatcher<QImage>(this);
    connect(m_watcherThumbnail, SIGNAL(resultReadyAt(int)), this, SLOT(showThumbnail(int)));
}

FileInfo ViewFiles::getCurrentFileInfo() const
{
    return m_currentInfo;
}

void ViewFiles::setModel(QAbstractItemModel *model)
{
    m_proxy->setSourceModel(model);
    QListView::setModel(m_proxy);
}

QModelIndex ViewFiles::getIndexFromProxy(const QModelIndex &index) const
{
    return m_proxy->mapToSource(index);
}

void ViewFiles::setCurrentIndexFromSource(const QModelIndex &index)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::setCurrentIndexFromSource" << index.data();
#endif
    QListView::setCurrentIndex(m_proxy->mapFromSource(index));
}

void ViewFiles::setCurrentDirectory(const FileInfo &info)
{
    m_currentInfo = info;

    if (this->viewMode() == QListView::IconMode)
    {
        this->startShowingThumbnails();
    }
}

void ViewFiles::setThumbnailsSize(const QSize &size)
{
    if (m_thumb_size != size)
    {
        m_thumb_size = size;
        if (this->viewMode() == QListView::IconMode)
        {
            this->setViewMode(QListView::IconMode);
            this->startShowingThumbnails();
        }
    }
}


/* This is index from ArchiveModel */
void ViewFiles::on_archiveDirsView_currentRowChanged(const QModelIndex &index)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::on_archiveDirsView_currentRowChanged" << index.data();
#endif
    if (!index.isValid())
    {
        return;
    }

    QString pathToImage = "";
    QModelIndex cindex = index;
    while (cindex.parent().isValid())
    {
        pathToImage = cindex.data(Qt::DisplayRole).toString() + "/" + pathToImage;
        cindex = cindex.parent();
    }

    m_currentInfo.zipPath = pathToImage;
    m_currentInfo.zipImageFileName.clear();
    this->setRootIndex(m_proxy->mapFromSource(index));
    this->selectionModel()->clear();

    this->startShowingThumbnails();
    emit currentFileChanged(m_currentInfo);
}

void ViewFiles::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    this->scrollTo(current);
    int type = current.data(ROLE_TYPE).toInt();
    QString filename = current.data(Qt::DisplayRole).toString();

#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::currentChanged" << filename;
#endif

    if (type == TYPE_FILE)
    {
        m_currentInfo.imageFileName = filename;
        m_currentInfo.zipImageFileName.clear();
        m_currentInfo.zipPath.clear();
    }
    else if (type == TYPE_ARCHIVE_FILE)
    {
        m_currentInfo.zipImageFileName = filename;
    }
    else
    {
        m_currentInfo.zipImageFileName.clear();
        m_currentInfo.imageFileName.clear();
    }

    emit currentFileChanged(m_currentInfo);
}

void ViewFiles::setViewMode(ViewMode mode)
{
//    if (mode == this->viewMode()) return;

    QSize iconSize;
    QSize gridSize;

    if (mode == QListView::IconMode)
    {
        iconSize = m_thumb_size;
        gridSize = QSize(iconSize.width() + 50, iconSize.height() + 50);
    }
    else
    {
        iconSize = QSize(-1, -1);
        gridSize = QSize(-1, -1);
        QFileIconProvider fip;
        for (int i = 0; i < m_proxy->rowCount(this->rootIndex()); ++i)
        {
            QString text = m_proxy->data(m_proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
            m_proxy->setData(m_proxy->index(i, 0, this->rootIndex()), QVariant(), Qt::SizeHintRole);
            m_proxy->setData(m_proxy->index(i, 0, this->rootIndex()), fip.icon(m_currentInfo.containerPath + "/" + text), Qt::DecorationRole);
        }
    }
    QListView::setViewMode(mode);

    this->setIconSize(iconSize);
    this->setGridSize(gridSize);

    this->setCurrentDirectory(m_currentInfo);
}

void ViewFiles::startShowingThumbnails()
{
    if (m_proxy->rowCount(this->rootIndex()) == 0 || this->viewMode() != QListView::IconMode)
    {
        return;
    }


    QList <ThumbnailInfo> files;
    for (int i = 0; i < m_proxy->rowCount(this->rootIndex()); ++i)
    {
        m_proxy->setData(m_proxy->index(i, 0, this->rootIndex()), QSize(m_thumb_size.width() + 20, m_thumb_size.height() + 20), Qt::SizeHintRole);
        FileInfo pli_info;

        int type = m_proxy->data(m_proxy->index(i, 0, this->rootIndex()), ROLE_TYPE).toInt();
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            pli_info = m_currentInfo;

            if (!m_currentInfo.isZip())
            {
                pli_info.imageFileName = m_proxy->data(m_proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
            }
            else
            {
                pli_info.zipImageFileName = m_proxy->data(m_proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
            }
        }
        files.append(ThumbnailInfo(pli_info, m_thumb_size));
    }

    if (m_watcherThumbnail->isRunning())
    {
        m_watcherThumbnail->cancel();
        m_watcherThumbnail->waitForFinished();
    }
    m_watcherThumbnail->setFuture(QtConcurrent::mapped(files, PictureLoader::getThumbnail));
}

void ViewFiles::showThumbnail(int num)
{
    if (!m_watcherThumbnail->resultAt(num).isNull())
    {
        m_proxy->setData(m_proxy->index(num, 0, this->rootIndex()), QPixmap::fromImage(m_watcherThumbnail->resultAt(num)), Qt::DecorationRole);
    }
    if (++m_returnThumbCount == m_proxy->rowCount(this->rootIndex()))
    {
        m_returnThumbCount = 0;
        m_watcherThumbnail->setFuture(QFuture<QImage>());
    }
}

void ViewFiles::pageNext()
{
    if (!this->currentIndex().isValid()) return;

    for (int i = this->currentIndex().row() + 1; i < m_proxy->rowCount(this->rootIndex()); ++i)
    {
        int type = m_proxy->index(i, 0, this->rootIndex()).data(ROLE_TYPE).toInt();
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            this->setCurrentIndex(m_proxy->index(i, 0, this->rootIndex()));
            break;
        }
    }
}

void ViewFiles::pagePrevious()
{
    if (!this->currentIndex().isValid()) return;

    for (int i = this->currentIndex().row() - 1; i >= 0; --i)
    {
        int type = m_proxy->index(i, 0, this->rootIndex()).data(ROLE_TYPE).toInt();
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            this->setCurrentIndex(m_proxy->index(i, 0, this->rootIndex()));
            break;
        }
    }
}
