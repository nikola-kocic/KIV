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
//    this->setViewMode(QListView::IconMode);
    threadThumbnails = 0;
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
    thumbCount = 0;

    if(this->viewMode() == QListView::IconMode)
    {
        if(threadThumbnails == 0)
        {
            threadThumbnails = new QThread();
            pl = new PixmapLoader();
            pl->moveToThread(threadThumbnails);
            pl->setThumbnailSize(this->thumbSize);
            connect(threadThumbnails, SIGNAL(started()), pl, SLOT(loadPixmap()));
            connect(threadThumbnails, SIGNAL(finished()), this, SLOT(onThreadThumbsFinished()));
            connect(pl, SIGNAL(finished(QPixmap)), this, SLOT(onThumbnailFinished(QPixmap)));
        }
        folderChangedFlag = true;
        if(threadThumbnails->isRunning() == false)
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
        QString text = proxy->data(proxy->index(thumbCount, 0, this->rootIndex()), Qt::DisplayRole).toString();

        for( int i = 0; i < proxy->rowCount(this->rootIndex()); i++ )
        {
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
    if(this->viewMode() == QListView::IconMode)
    {
        for( int i = 0; i < proxy->rowCount(this->rootIndex()); i++ )
        {
            proxy->setData(proxy->index(i, 0, this->rootIndex()), QSize(this->thumbSize + 20, this->thumbSize + 20), Qt::SizeHintRole);
        }
    }

    //#TODO find better way to update content (as when resized)
    this->reset();

    showThumbnail();
}

void ThumbnailViewer::showThumbnail()
{
    if(proxy->rowCount(this->rootIndex()) == 0)
    {
        return;
    }
    while(true)
    {
        if(proxy->data(proxy->index(thumbCount, 0, this->rootIndex()), ROLE_TYPE).toInt() == TYPE_FILE
                || (proxy->data(proxy->index(thumbCount, 0, this->rootIndex()), ROLE_TYPE).toInt() >= makeArchiveNumberForItem(0)))
        {
            break;
        }

        thumbCount++;
        if(thumbCount >= proxy->rowCount(this->rootIndex()))
        {
            return;
        }
    }

    QString text = proxy->data(proxy->index(thumbCount, 0, this->rootIndex()), Qt::DisplayRole).toString();
    if(this->isZip == true)
    {
        pl->setFilePath(path, true, text);
    }
    else
    {
        pl->setFilePath(path + "/" + text);
    }

    threadThumbnails->start();
}

void ThumbnailViewer::onThreadThumbsFinished()
{
    if(thumbCount < proxy->rowCount(this->rootIndex()) - 1 && folderChangedFlag == false)
    {
        thumbCount++;
        showThumbnail();
    }
    else
    {
        thumbCount = 0;
        if(folderChangedFlag == true)
        {
            folderChangedFlag = false;
            startShowingThumbnails();
        }
    }
}

void ThumbnailViewer::onThumbnailFinished(QPixmap thumb)
{
    if(!thumb.isNull())
    {
//        qDebug() << thumbSize;

//        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignTop);
//        proxy->setData(proxy->index(thumbCount, 0, this->rootIndex()), (Qt::AlignHCenter | Qt::AlignTop), Qt::TextAlignmentRole);
//        this->update(proxy->index(thumbCount, 0, this->rootIndex()));
        qDebug() <<  proxy->data(proxy->index(thumbCount, 0, this->rootIndex()), Qt::SizeHintRole).toSize()
                  << proxy->data(proxy->index(thumbCount, 0, this->rootIndex()), Qt::DisplayRole).toString();
        proxy->setData(proxy->index(thumbCount, 0, this->rootIndex()), thumb, Qt::DecorationRole);
    }
    threadThumbnails->exit();
}
