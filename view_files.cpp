#include "view_files.h"
#include "settings.h"

#include <QtCore/qdir.h>
#include <QtGui/qfileiconprovider.h>
#include <QtGui/qapplication.h>
#include <QtGui/qstyleditemdelegate.h>

ViewFiles::ViewFiles(QWidget *parent)
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->proxy = new QSortFilterProxyModel(this);
    this->currentInfo.thumbSize = Settings::Instance()->getThumbnailSize();

    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setUniformItemSizes(true);

    this->returnThumbCount = 0;
    this->watcherThumbnail = new QFutureWatcher<QImage>(this);
    connect(this->watcherThumbnail, SIGNAL(resultReadyAt(int)), this, SLOT(showThumbnail(int)));
}

FileInfo ViewFiles::getCurrentFileInfo()
{
    return this->currentInfo;
}

void ViewFiles::setModel(QAbstractItemModel *model)
{
    this->proxy->setSourceModel(model);
    QListView::setModel(this->proxy);
}

QModelIndex ViewFiles::getIndexFromProxy(const QModelIndex &index)
{
    return this->proxy->mapToSource(index);
}

void ViewFiles::setCurrentDirectory(const FileInfo &info)
{
    this->currentInfo = info;

    if (this->viewMode() == QListView::IconMode)
    {
        if (this->watcherThumbnail->isRunning())
        {
            this->watcherThumbnail->cancel();
            this->watcherThumbnail->waitForFinished();
        }
        this->startShowingThumbnails();
    }
}


//This is index from ArchiveModel
void ViewFiles::OnTreeViewArchiveDirsCurrentChanged(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }

    this->currentInfo.zipPathToImage.clear();
    QModelIndex cindex = index;
    while (cindex.parent().isValid())
    {
        this->currentInfo.zipPathToImage = cindex.data(Qt::DisplayRole).toString() + "/" + this->currentInfo.zipPathToImage;
        cindex = cindex.parent();
    }

    this->setRootIndex(this->proxy->mapFromSource(index));

    this->startShowingThumbnails();
}

void ViewFiles::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QListView::currentChanged(current, previous);

    int type = current.data(ROLE_TYPE).toInt();
    QString filename = current.data(Qt::DisplayRole).toString();

    if (type == TYPE_FILE)
    {
        this->currentInfo.imageFileName = filename;
        this->currentInfo.zipImageFileName.clear();
        this->currentInfo.zipPathToImage.clear();
    }
    if (type == TYPE_ARCHIVE_FILE)
    {
        this->currentInfo.zipImageFileName = filename;
    }

    FileInfo info = this->currentInfo;
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
        this->currentInfo.thumbSize = Settings::Instance()->getThumbnailSize();
        iconSize = this->currentInfo.thumbSize;
        gridSize = iconSize + 50;
    }
    else
    {
        QAbstractItemDelegate *delegate = this->itemDelegate();
        QSize size = delegate->sizeHint(QStyleOptionViewItem (), this->model()->parent(this->rootIndex()));

        QFileIconProvider fip;

        for (int i = 0; i < this->proxy->rowCount(this->rootIndex()); ++i)
        {
            QString text = this->proxy->data(this->proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
            this->proxy->setData(this->proxy->index(i, 0, this->rootIndex()), size, Qt::SizeHintRole);
            this->proxy->setData(this->proxy->index(i, 0, this->rootIndex()), fip.icon(this->currentInfo.containerPath + "/" + text), Qt::DecorationRole);
        }
    }
    QListView::setViewMode(mode);

    this->setIconSize(QSize(iconSize, iconSize));
    this->setGridSize(QSize(gridSize, gridSize));

    this->setCurrentDirectory(this->currentInfo);
}

void ViewFiles::startShowingThumbnails()
{
    if (this->proxy->rowCount(this->rootIndex()) == 0 || this->viewMode() != QListView::IconMode)
    {
        return;
    }

    QList <FileInfo> files;
    for (int i = 0; i < this->proxy->rowCount(this->rootIndex()); ++i)
    {
        this->proxy->setData(this->proxy->index(i, 0, this->rootIndex()), QSize(this->currentInfo.thumbSize + 20, this->currentInfo.thumbSize + 20), Qt::SizeHintRole);

        if (this->proxy->data(this->proxy->index(i, 0, this->rootIndex()), ROLE_TYPE).toInt() == TYPE_FILE
                || (this->proxy->data(this->proxy->index(i, 0, this->rootIndex()), ROLE_TYPE).toInt() == TYPE_ARCHIVE_FILE))
        {
            FileInfo info = this->currentInfo;

            if (this->currentInfo.zipPathToImage.isEmpty())
            {
                info.imageFileName = this->proxy->data(this->proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
            }
            else
            {
                info.zipImageFileName = this->proxy->data(this->proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
            }

            files.append(info);
        }
        else
        {
            FileInfo info;
            info.thumbSize = this->currentInfo.thumbSize;
            files.append(info);
        }
    }

//    this->reset();

    this->watcherThumbnail->setFuture(QtConcurrent::mapped(files, PictureLoader::getImage));
}

void ViewFiles::showThumbnail(int num)
{
    if (!this->watcherThumbnail->resultAt(num).isNull())
    {
        this->proxy->setData(this->proxy->index(num, 0, this->rootIndex()), QPixmap::fromImage(this->watcherThumbnail->resultAt(num)), Qt::DecorationRole);
    }
    if(++this->returnThumbCount == this->proxy->rowCount(this->rootIndex()))
    {
        this->returnThumbCount = 0;
        this->watcherThumbnail->setFuture(QFuture<QImage>());
    }
}

void ViewFiles::pageNext()
{
    if (!this->currentIndex().isValid()) return;

    for (int i = this->currentIndex().row() + 1; i < this->proxy->rowCount(this->rootIndex()); ++i)
    {
        int type = this->proxy->index(i, 0, this->rootIndex()).data(ROLE_TYPE).toInt();
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            this->setCurrentIndex(this->proxy->index(i, 0, this->rootIndex()));
            break;
        }
    }
}

void ViewFiles::pagePrevious()
{
    if (!this->currentIndex().isValid()) return;

    for (int i = this->currentIndex().row() - 1; i >= 0; --i)
    {
        int type = this->proxy->index(i, 0, this->rootIndex()).data(ROLE_TYPE).toInt();
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            this->setCurrentIndex(this->proxy->index(i, 0, this->rootIndex()));
            break;
        }
    }
}
