#include "thumbnail_viewer.h"

#include <QtCore/qdir.h>
#include <QtGui/qfileiconprovider.h>
#include <QtGui/qapplication.h>

#include <QtCore/QDebug>

ThumbnailViewer::ThumbnailViewer(QWidget * parent)
{
    this->fsm = new QFileSystemModel();
    folderChangedFlag = false;
    this->thumbSize = 200;

    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setUniformItemSizes(true);
//    this->setViewMode(QListView::IconMode);
    threadThumbnails = 0;
    this->setModel(fsm);
}

void ThumbnailViewer::setCurrentDirectory(const QString &path, bool isZip, const QString &zipFileName)
{
    this->isZip = isZip;
    this->zipFileName = zipFileName;
    this->setRootIndex(fsm->index(path));
}

void ThumbnailViewer::setViewMode(ViewMode mode)
{
    int iconSize = -1;
    int gridSize = -1;

    if(mode == QListView::IconMode)
    {
        iconSize = thumbSize;
        gridSize = thumbSize + 50;
//        for( int i = 0; i < this->count(); i++ )
//        {
//            this->item(i)->setSizeHint(QSize(this->thumbSize + 20, this->thumbSize + 20));
//            this->item(i)->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
//        }
    }
    else
    {
//        QModelIndex index = this->indexFromItem(this->item(-1));
//        QAbstractItemDelegate * delegate = this->itemDelegate();
//        QSize size = delegate->sizeHint( QStyleOptionViewItem (), index );

//        for( int i = 0; i < this->count(); i++ )
//        {
//            this->item(i)->setSizeHint(size);
//            this->item(i)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
//        }
    }

    QListView::setViewMode( mode );

    this->setIconSize( QSize( iconSize, iconSize ) );
    this->setGridSize( QSize( gridSize, gridSize) );
}

//void ThumbnailViewer::addItem(QListWidgetItem *item)
//{
//    if(this->viewMode() == QListView::IconMode)
//    {
//        item->setSizeHint(QSize(this->thumbSize + 20, this->thumbSize + 20));
//        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
//    }
//    item->setToolTip(item->text());

//    QListWidget::addItem(item);
//}

//void ThumbnailViewer::startShowingThumbnails(const QString& path, bool isZip)
//{
//    if(threadThumbnails == 0)
//    {
//        qDebug("creating and connecting threadThumbnails");
//        threadThumbnails = new QThread();
//        pl = new PixmapLoader();
//        pl->moveToThread(threadThumbnails);
//        pl->setThumbnailSize(this->thumbSize);
//        connect(threadThumbnails, SIGNAL(started()), pl, SLOT(loadPixmap()));
//        connect(threadThumbnails, SIGNAL(finished()), this, SLOT(onThreadThumbsFinished()));
//        connect(pl, SIGNAL(finished(QIcon)), this, SLOT(onThumbnailFinished(QIcon)));
//    }

//    this->isZip = isZip;
//    this->zipFileName = zipFileName;
//    if(this->path != path)
//    {
//        folderChangedFlag = true;
//        thumbCount = 0;
//        this->path = path;

//        if(threadThumbnails->isRunning() == false)
//        {
//            showThumbnail();
//        }
//    }
//}

void ThumbnailViewer::showThumbnail()
{
//    if(this->count() == 0)
//    {
//        return;
//    }
//    while(true)
//    {
//        if(this->item(thumbCount)->type() == TYPE_FILE || this->item(thumbCount)->type() >= makeArchiveNumberForItem(0))
//        {
//            break;
//        }

//        thumbCount++;
//        if(thumbCount >= this->count())
//        {
//            return;
//        }
//    }

//    if(this->isZip == true)
//    {
//        pl->setFilePath(path, true, this->item(thumbCount)->text());
//    }
//    else
//    {
//        pl->setFilePath(path + "/" + this->item(thumbCount)->text());
//    }
    threadThumbnails->start();
}

void ThumbnailViewer::onThreadThumbsFinished()
{
//    if(thumbCount < this->count() - 1 && folderChangedFlag == false)
//    {
//        thumbCount++;
//        showThumbnail();
//    }
//    else
//    {
//        thumbCount = 0;
//        if(folderChangedFlag == true)
//        {
//            folderChangedFlag = false;
//            showThumbnail();
//        }
//    }
}

void ThumbnailViewer::onThumbnailFinished(QIcon icon)
{
    if(!icon.isNull())
    {
//        this->item(thumbCount)->setIcon(icon);
    }
    threadThumbnails->exit();
}
