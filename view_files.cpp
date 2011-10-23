#include "view_files.h"
#include "settings.h"

#include <QtCore/qdir.h>
#include <QtGui/qfileiconprovider.h>
#include <QtGui/qapplication.h>
#include <QtGui/qstyleditemdelegate.h>

ViewFiles::ViewFiles(ArchiveModel *am, QWidget * parent)
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    proxy = new QSortFilterProxyModel();
    this->am = am;
    folderChangedFlag = false;
    this->currentInfo.thumbSize = Settings::Instance()->getThumbnailSize();

    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setUniformItemSizes(true);

    imageScaling = new QFutureWatcher<QPixmap>(this);
    connect(imageScaling, SIGNAL(resultReadyAt(int)), this, SLOT(showImage(int)));


    this->setModel(am);
}

FileInfo ViewFiles::getCurrentFileInfo()
{
    return currentInfo;
}

void ViewFiles::setModel(QAbstractItemModel * model)
{
    proxy->setSourceModel(model);
    QListView::setModel(proxy);
}

QModelIndex ViewFiles::getIndexFromProxy(const QModelIndex & index)
{
    return proxy->mapToSource(index);
}

void ViewFiles::setCurrentDirectory(const FileInfo &info)
{
    this->currentInfo = info;

    if (this->viewMode() == QListView::IconMode)
    {
        folderChangedFlag = true;
        if (imageScaling->isRunning())
        {
            imageScaling->cancel();
            imageScaling->waitForFinished();
        }
        startShowingThumbnails();
    }
}


//This is index from ArchiveModel
void ViewFiles::OnTreeViewArchiveDirsCurrentChanged(const QModelIndex & index)
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

    this->setRootIndex(proxy->mapFromSource(index));

    startShowingThumbnails();
}

void ViewFiles::currentChanged(const QModelIndex & current, const QModelIndex & previous)
{
    QListView::currentChanged(current, previous);

    int type = current.data(ROLE_TYPE).toInt();
    QString filename = current.data(Qt::DisplayRole).toString();

    if (type == TYPE_FILE)
    {
        currentInfo.imageFileName = filename;
        currentInfo.zipImageFileName.clear();
        currentInfo.zipPathToImage.clear();
    }
    if (type == TYPE_ARCHIVE_FILE)
    {
        currentInfo.zipImageFileName = filename;
    }

    emit currentFileChanged(currentInfo);
}

void ViewFiles::setViewMode(ViewMode mode)
{
    if (mode == this->viewMode()) return;

    int iconSize = -1;
    int gridSize = -1;

    if (mode == QListView::IconMode)
    {
        currentInfo.thumbSize = Settings::Instance()->getThumbnailSize();
        iconSize = currentInfo.thumbSize;
        gridSize = iconSize + 50;
    }
    else
    {
        QAbstractItemDelegate * delegate = this->itemDelegate();
        QSize size = delegate->sizeHint(QStyleOptionViewItem (), am->invisibleRootItem()->index());

        QFileIconProvider fip;

        for(int i = 0; i < proxy->rowCount(this->rootIndex()); i++)
        {
            QString text = proxy->data(proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
            proxy->setData(proxy->index(i, 0, this->rootIndex()), size, Qt::SizeHintRole);
            proxy->setData(proxy->index(i, 0, this->rootIndex()), fip.icon(currentInfo.containerPath + "/" + text), Qt::DecorationRole);
        }
    }
    QListView::setViewMode(mode);

    this->setIconSize(QSize(iconSize, iconSize));
    this->setGridSize(QSize(gridSize, gridSize));

    setCurrentDirectory(currentInfo);
}

void ViewFiles::startShowingThumbnails()
{
    if (proxy->rowCount(this->rootIndex()) == 0 || this->viewMode() != QListView::IconMode)
    {
        return;
    }

    QList <FileInfo> files;
    for(int i = 0; i < proxy->rowCount(this->rootIndex()); i++)
    {
        proxy->setData(proxy->index(i, 0, this->rootIndex()), QSize(currentInfo.thumbSize + 20, currentInfo.thumbSize + 20), Qt::SizeHintRole);

        if (proxy->data(proxy->index(i, 0, this->rootIndex()), ROLE_TYPE).toInt() == TYPE_FILE
                || (proxy->data(proxy->index(i, 0, this->rootIndex()), ROLE_TYPE).toInt() == TYPE_ARCHIVE_FILE))
        {
            FileInfo info = currentInfo;

            if (currentInfo.zipPathToImage.isEmpty())
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
            info.thumbSize = currentInfo.thumbSize;
            files.append(info);
        }
    }

//    this->reset();

    imageScaling->setFuture(QtConcurrent::mapped(files, PictureLoader::getPixmap));
}

void ViewFiles::showImage(int num)
{
    if (!imageScaling->resultAt(num).isNull())
    {
        proxy->setData(proxy->index(num, 0, this->rootIndex()), imageScaling->resultAt(num), Qt::DecorationRole);
    }
}

void ViewFiles::pageNext()
{
    if (!this->currentIndex().isValid()) return;

    for (int i = this->currentIndex().row() + 1; i < proxy->rowCount(this->rootIndex()); ++i)
    {
        int type = proxy->index(i, 0, this->rootIndex()).data(ROLE_TYPE).toInt();
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            this->setCurrentIndex(proxy->index(i, 0, this->rootIndex()));
            break;
        }
    }
}

void ViewFiles::pagePrevious()
{
    if (!this->currentIndex().isValid()) return;

    for (int i = this->currentIndex().row() - 1; i >= 0; --i)
    {
        int type = proxy->index(i, 0, this->rootIndex()).data(ROLE_TYPE).toInt();
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            this->setCurrentIndex(proxy->index(i, 0, this->rootIndex()));
            break;
        }
    }
}
