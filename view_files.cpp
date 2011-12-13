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
    m_currentInfo.thumbSize = Settings::Instance()->getThumbnailSize();

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
        m_currentInfo.thumbSize = Settings::Instance()->getThumbnailSize();
        this->startShowingThumbnails();
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


    FileInfo info = m_currentInfo;
    info.thumbSize = 0;
    emit currentFileChanged(info);
}

void ViewFiles::setViewMode(ViewMode mode)
{
    if (mode == this->viewMode()) return;

    int iconSize = -1;
    int gridSize = -1;

    if (mode == QListView::IconMode)
    {
        m_currentInfo.thumbSize = Settings::Instance()->getThumbnailSize();
        iconSize = m_currentInfo.thumbSize;
        gridSize = iconSize + 50;
    }
    else
    {
        QFileIconProvider fip;
        for (int i = 0; i < m_proxy->rowCount(this->rootIndex()); ++i)
        {
            QString text = m_proxy->data(m_proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
            m_proxy->setData(m_proxy->index(i, 0, this->rootIndex()), QVariant(), Qt::SizeHintRole);
            m_proxy->setData(m_proxy->index(i, 0, this->rootIndex()), fip.icon(m_currentInfo.containerPath + "/" + text), Qt::DecorationRole);
        }
    }
    QListView::setViewMode(mode);

    this->setIconSize(QSize(iconSize, iconSize));
    this->setGridSize(QSize(gridSize, gridSize));

    this->setCurrentDirectory(m_currentInfo);
}

void ViewFiles::startShowingThumbnails()
{
    if (m_proxy->rowCount(this->rootIndex()) == 0 || this->viewMode() != QListView::IconMode)
    {
        return;
    }


    QList <FileInfo> files;
    for (int i = 0; i < m_proxy->rowCount(this->rootIndex()); ++i)
    {
        m_proxy->setData(m_proxy->index(i, 0, this->rootIndex()), QSize(m_currentInfo.thumbSize + 20, m_currentInfo.thumbSize + 20), Qt::SizeHintRole);
        FileInfo info;
        if (m_proxy->data(m_proxy->index(i, 0, this->rootIndex()), ROLE_TYPE).toInt() == TYPE_FILE
                || (m_proxy->data(m_proxy->index(i, 0, this->rootIndex()), ROLE_TYPE).toInt() == TYPE_ARCHIVE_FILE))
        {
            info = m_currentInfo;

            if (!m_currentInfo.isZip())
            {
                info.imageFileName = m_proxy->data(m_proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
            }
            else
            {
                info.zipImageFileName = m_proxy->data(m_proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
            }
        }
        else
        {
            info.thumbSize = m_currentInfo.thumbSize;
        }
        files.append(info);
    }

    if (m_watcherThumbnail->isRunning())
    {
        m_watcherThumbnail->cancel();
        m_watcherThumbnail->waitForFinished();
    }
    m_watcherThumbnail->setFuture(QtConcurrent::mapped(files, PictureLoader::getImage));
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
