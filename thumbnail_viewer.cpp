#include "thumbnail_viewer.h"
#include "settings.h"

#include <QtCore/qdir.h>
#include <QtGui/qfileiconprovider.h>
#include <QtGui/qapplication.h>
#include <QtGui/qstyleditemdelegate.h>

#include <QtCore/QDebug>

ThumbnailViewer::ThumbnailViewer(ArchiveModel *am, QWidget * parent)
{
    proxy = new QSortFilterProxyModel();
    this->am = am;
    folderChangedFlag = false;
    thumbSize = Settings::Instance()->getThumbnailSize();

    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setUniformItemSizes(true);

    imageScaling = new QFutureWatcher<QPixmap>(this);
    connect(imageScaling, SIGNAL(resultReadyAt(int)), SLOT(showImage(int)));
    connect(imageScaling, SIGNAL(finished()), SLOT(finished()));

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
    this->zipFileName = zipFileName;
    this->path = filePath;

    if(this->viewMode() == QListView::IconMode)
    {
        folderChangedFlag = true;
        if(imageScaling->isRunning() == false)
        {
            startShowingThumbnails();
        }
    }
}

void ThumbnailViewer::OnTreeViewArchiveDirsCurrentChanged ( const QModelIndex & index )
{
    if(index.isValid() == false)
    {
        return;
    }

    this->setRootIndex(proxy->mapFromSource(index));
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

    setCurrentDirectory(this->path, this->isZip, this->zipFileName);
}

void ThumbnailViewer::startShowingThumbnails()
{
    if(proxy->rowCount(this->rootIndex()) == 0)
    {
        return;
    }

    t.start();
    if(this->isZip == false)
    {
        QList <ZipInfo> files;
        if(this->viewMode() == QListView::IconMode)
        {
            for( int i = 0; i < proxy->rowCount(this->rootIndex()); i++ )
            {
                proxy->setData(proxy->index(i, 0, this->rootIndex()), QSize(this->thumbSize + 20, this->thumbSize + 20), Qt::SizeHintRole);

                if(proxy->data(proxy->index(i, 0, this->rootIndex()), ROLE_TYPE).toInt() == TYPE_FILE
                        || (proxy->data(proxy->index(i, 0, this->rootIndex()), ROLE_TYPE).toInt() >= makeArchiveNumberForItem(0)))
                {
                    ZipInfo info;
                    info.filePath = path + "/" + proxy->data(proxy->index(i, 0, this->rootIndex()), Qt::DisplayRole).toString();
                    info.zipFile = "";
                    info.thumbSize = this->thumbSize;
                    files.append(info);
                }
            }
        }

    //    if(this->isZip == true)
    //    {
    //        pl->setFilePath(path, true, text);
    //    }
    //    else
    //    {
    //        pl->setFilePath(path + "/" + text);
    //    }

        //#TODO find better way to update content (as when resized)
        this->reset();



        imageScaling->setFuture(QtConcurrent::mapped(files, loadFromFile));

    }
//    showThumbnail();
}

void ThumbnailViewer::finished()
{
    qDebug() << t.elapsed();
}

void ThumbnailViewer::showImage(int num)
{
    if(!imageScaling->resultAt(num).isNull())
    {
        qDebug() << t.elapsed();
        proxy->setData(proxy->index(num, 0, this->rootIndex()), imageScaling->resultAt(num), Qt::DecorationRole);
    }
}
