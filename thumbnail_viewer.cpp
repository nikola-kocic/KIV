#include "thumbnail_viewer.h"
#include "settings.h"

#include <QtCore/qdir.h>
#include <QtGui/qfileiconprovider.h>
#include <QtGui/qapplication.h>
#include <QtGui/qstyleditemdelegate.h>

#include <QtCore/QDebug>

ThumbnailViewer::ThumbnailViewer(ArchiveModel *am, QWidget * parent)
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    proxy = new QSortFilterProxyModel();
    this->am = am;
    folderChangedFlag = false;
    thumbSize = Settings::Instance()->getThumbnailSize();

    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setUniformItemSizes(true);

    imageScaling = new QFutureWatcher<QPixmap>(this);
    connect(imageScaling, SIGNAL(resultReadyAt(int)), this, SLOT(showImage(int)));

    imageLoad = new QFutureWatcher<QPixmap>(this);
    connect(imageLoad, SIGNAL(resultReadyAt(int)), this, SLOT(imageFinished(int)));

    this->setModel(am);
}

void ThumbnailViewer::setModel ( QAbstractItemModel * model )
{
    proxy->setSourceModel(model);
    QListView::setModel(proxy);
}

void ThumbnailViewer::setCurrentDirectory(const QString &filePath, bool isZip, const QString &zipFileName)
{
    this->isZip = isZip;
    this->zipPath = zipFileName;
    this->path = filePath;

    if(this->viewMode() == QListView::IconMode)
    {
        folderChangedFlag = true;
        if(imageScaling->isRunning() == false)
        {
            imageScaling->cancel();
            imageScaling->waitForFinished();
        }
        startShowingThumbnails();
    }
}


//This is index from ArchiveModel
void ThumbnailViewer::OnTreeViewArchiveDirsCurrentChanged ( const QModelIndex & index )
{
//    qDebug() << index;
    if(index.isValid() == false)
    {
        return;
    }

    this->zipPath = "";
    QModelIndex cindex = index;
    while(cindex.parent().isValid() == true)
    {
        this->zipPath = cindex.data(Qt::DisplayRole).toString() + "/" + this->zipPath;
//        qDebug() << this->zipPath;
        cindex = cindex.parent();
    }

    this->setRootIndex(proxy->mapFromSource(index));

    startShowingThumbnails();
}

void ThumbnailViewer::currentChanged ( const QModelIndex & current, const QModelIndex & previous )
{
    QListView::currentChanged(current, previous);

    int type = current.data(ROLE_TYPE).toInt();
    QString filename = current.data(Qt::DisplayRole).toString();

    if(type == TYPE_DIR || type == TYPE_ARCHIVE)
    {
        emit imageLoaded(QPixmap(0,0));
    }
    else
    {
        if(type == TYPE_FILE)
        {
            ZipInfo info;
            info.filePath = this->path + "/" + filename;
            info.zipFile = "";
            info.thumbSize = 0;

            imageLoad->setFuture(QtConcurrent::run(loadFromFile, info));
        }
        else if(type == TYPE_ARCHIVE_FILE)
        {
            ZipInfo info;
            info.filePath = this->path;
            info.zipFile = this->zipPath + filename;
            info.thumbSize = 0;

            imageLoad->setFuture(QtConcurrent::run(loadFromZip, info));
        }
        else
        {
            emit imageLoaded(QPixmap(0,0));
        }
    }
}

void ThumbnailViewer::imageFinished(int num)
{
    emit imageLoaded(imageLoad->resultAt(num));
}

void ThumbnailViewer::setViewMode(ViewMode mode)
{
    if(mode == this->viewMode()) return;

    int iconSize = -1;
    int gridSize = -1;

    if(mode == QListView::IconMode)
    {
        thumbSize = Settings::Instance()->getThumbnailSize();
        iconSize = thumbSize;
        gridSize = thumbSize + 50;
    }
    else
    {
        QAbstractItemDelegate * delegate = this->itemDelegate();
        QSize size = delegate->sizeHint( QStyleOptionViewItem (), am->invisibleRootItem()->index());

        QFileIconProvider fip;

        for( int i = 0; i < proxy->rowCount(this->rootIndex()); i++ )
        {
            QString text = proxy->data(proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
            proxy->setData(proxy->index(i, 0, this->rootIndex()), size, Qt::SizeHintRole);
            proxy->setData(proxy->index(i, 0, this->rootIndex()), fip.icon(path + "/" + text), Qt::DecorationRole);
        }
    }
    QListView::setViewMode( mode );

    this->setIconSize( QSize( iconSize, iconSize ) );
    this->setGridSize( QSize( gridSize, gridSize) );

    setCurrentDirectory(this->path, this->isZip, this->zipPath);
}

void ThumbnailViewer::startShowingThumbnails()
{
    if(proxy->rowCount(this->rootIndex()) == 0 || this->viewMode() != QListView::IconMode)
    {
        return;
    }

    QList <ZipInfo> files;
    for( int i = 0; i < proxy->rowCount(this->rootIndex()); i++ )
    {
        proxy->setData(proxy->index(i, 0, this->rootIndex()), QSize(this->thumbSize + 20, this->thumbSize + 20), Qt::SizeHintRole);

        if(proxy->data(proxy->index(i, 0, this->rootIndex()), ROLE_TYPE).toInt() == TYPE_FILE
                || (proxy->data(proxy->index(i, 0, this->rootIndex()), ROLE_TYPE).toInt() == TYPE_ARCHIVE_FILE))
        {
            ZipInfo info;

            if(this->isZip == true)
            {
                info.filePath = this->path;
                info.zipFile = this->zipPath + this->proxy->data(this->proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
            }
            else
            {
                info.filePath = this->path + "/" + this->proxy->data(this->proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
                info.zipFile = "";
            }
            info.thumbSize = this->thumbSize;

            files.append(info);
        }
        else
        {

            ZipInfo info;
            info.filePath = "";
            info.zipFile ="";
            info.thumbSize = this->thumbSize;
            files.append(info);
        }
    }

//    this->reset();

    if(this->isZip == true)
    {
        imageScaling->setFuture(QtConcurrent::mapped(files, loadFromZip));
    }
    else
    {
        imageScaling->setFuture(QtConcurrent::mapped(files, loadFromFile));
    }
}

void ThumbnailViewer::showImage(int num)
{
    if(!imageScaling->resultAt(num).isNull())
    {
        proxy->setData(proxy->index(num, 0, this->rootIndex()), imageScaling->resultAt(num), Qt::DecorationRole);
    }
}
