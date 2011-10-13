#include "thumbnail_viewer.h"

#include <QtCore/qdir.h>
#include <QtGui/qfileiconprovider.h>
#include <QtGui/qapplication.h>

#include <QtCore/QDebug>

ThumbnailViewer::ThumbnailViewer(QWidget * parent)
{
    this->model = new QStandardItemModel();
    folderChangedFlag = false;
    this->thumbSize = 200;

    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setUniformItemSizes(true);
//    this->setViewMode(QListView::IconMode);
    threadThumbnails = 0;
    this->setModel(model);
}

void ThumbnailViewer::setCurrentDirectory(const QString &filePath, bool isZip, const QString &zipFileName)
{
    this->isZip = isZip;
    this->zipFileName = zipFileName;
    model->clear();

    if(isZip == false)
    {
        QDir dir(filePath);

        QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
        QFileIconProvider fip;

        for (int i=0; i < list.count(); i++)
        {
            QStandardItem* item = 0;

            QFileInfo info = list.at(i);
            if(info.isDir())
            {
                item = new QStandardItem();
                item->setData(TYPE_DIR, ROLE_TYPE);
            }
            else if(isArchive(info))
            {
                item = new QStandardItem();
                item->setData(TYPE_ARCHIVE, ROLE_TYPE);
            }
            else if (isImage(info))
            {
                item = new QStandardItem();
                item->setData(TYPE_FILE, ROLE_TYPE);
            }

            if(item != 0)
            {
                item->setText(info.fileName());
                item->setIcon(fip.icon(info));
                if(this->viewMode() == QListView::IconMode)
                {
                    item->setSizeHint(QSize(this->thumbSize + 20, this->thumbSize + 20));
                    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
                }
                item->setToolTip(item->text());

                model->invisibleRootItem()->appendRow(item);
            }
        }
    }

    if(this->path != filePath)
    {
        this->path = filePath;
        thumbCount = 0;

        if(this->viewMode() == QListView::IconMode)
        {
            folderChangedFlag = true;
            if(threadThumbnails->isRunning() == false)
            {
                showThumbnail();
            }
        }
    }
}

void ThumbnailViewer::setViewMode(ViewMode mode)
{
    int iconSize = -1;
    int gridSize = -1;

    if(mode == QListView::IconMode)
    {
        folderChangedFlag = true;
        iconSize = thumbSize;
        gridSize = thumbSize + 50;
        for( int i = 0; i < model->invisibleRootItem()->rowCount(); i++ )
        {
            model->invisibleRootItem()->child(i)->setSizeHint(QSize(this->thumbSize + 20, this->thumbSize + 20));
            model->invisibleRootItem()->child(i)->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
        }

        startShowingThumbnails();
    }
    else
    {
        QAbstractItemDelegate * delegate = this->itemDelegate();
        QSize size = delegate->sizeHint( QStyleOptionViewItem (), model->invisibleRootItem()->child(0)->index() );

        for( int i = 0; i < model->invisibleRootItem()->rowCount(); i++ )
        {
            model->invisibleRootItem()->child(i)->setSizeHint(size);
            model->invisibleRootItem()->child(i)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }

    QListView::setViewMode( mode );

    this->setIconSize( QSize( iconSize, iconSize ) );
    this->setGridSize( QSize( gridSize, gridSize) );
}


void ThumbnailViewer::startShowingThumbnails()
{
    if(threadThumbnails == 0)
    {
        qDebug("creating and connecting threadThumbnails");
        threadThumbnails = new QThread();
        pl = new PixmapLoader();
        pl->moveToThread(threadThumbnails);
        pl->setThumbnailSize(this->thumbSize);
        connect(threadThumbnails, SIGNAL(started()), pl, SLOT(loadPixmap()));
        connect(threadThumbnails, SIGNAL(finished()), this, SLOT(onThreadThumbsFinished()));
        connect(pl, SIGNAL(finished(QIcon)), this, SLOT(onThumbnailFinished(QIcon)));
    }

}

void ThumbnailViewer::showThumbnail()
{
    if(model->invisibleRootItem()->rowCount() == 0)
    {
        return;
    }
    while(true)
    {
        if(model->invisibleRootItem()->child(thumbCount)->data(ROLE_TYPE).toInt() == TYPE_FILE
                || (model->invisibleRootItem()->child(thumbCount)->data(ROLE_TYPE).toInt() >= makeArchiveNumberForItem(0)))
        {
            break;
        }

        thumbCount++;
        if(thumbCount >= model->invisibleRootItem()->rowCount())
        {
            return;
        }
    }

    if(this->isZip == true)
    {
        pl->setFilePath(path, true, (model->invisibleRootItem()->child(thumbCount)->text()));
    }
    else
    {
        pl->setFilePath(path + "/" + (model->invisibleRootItem()->child(thumbCount)->text()));
    }
    threadThumbnails->start();
}

void ThumbnailViewer::onThreadThumbsFinished()
{
    if(thumbCount < model->invisibleRootItem()->rowCount() - 1 && folderChangedFlag == false)
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
            showThumbnail();
        }
    }
}

void ThumbnailViewer::onThumbnailFinished(QIcon icon)
{
    if(!icon.isNull())
    {
        model->invisibleRootItem()->child(thumbCount)->setIcon(icon);
    }
    threadThumbnails->exit();
}
