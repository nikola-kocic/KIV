#include "thumbnail_viewer.h"

#include <QtCore/qdir.h>
#include <QtGui/qfileiconprovider.h>
#include <QtGui/qapplication.h>
#include <QtGui/qstyleditemdelegate.h>

#include <QtCore/QDebug>

ThumbnailViewer::ThumbnailViewer(ArchiveModel *am, QWidget * parent)
{
    this->am = am;
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
                    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignTop);
                }
                item->setToolTip(item->text());

                model->invisibleRootItem()->appendRow(item);
            }
        }
    }
    else
    {

    }

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
            connect(pl, SIGNAL(finished(QIcon)), this, SLOT(onThumbnailFinished(QIcon)));
        }
        folderChangedFlag = true;
        if(threadThumbnails->isRunning() == false)
        {
            startShowingThumbnails();
        }
    }

}

void ThumbnailViewer::OnTreeViewArchiveDirsCurrentChanged ( const QModelIndex & current, const QModelIndex & previous )
{
    if(current.isValid() == false)
    {
        return;
    }


//    fileList->clear();
    if(current.data(ROLE_TYPE).toInt() == TYPE_ARCHIVE)
    {
//        for(int i=0; i < archive_files.count() ; i++)
//        {
//            if(!(archive_files.at(i).contains('/')))
//            {
//                fileList->addItem(new QListWidgetItem(archive_files.at(i), 0, makeArchiveNumberForItem(i)));
//            }
//        }
    }
    else
    {
        QModelIndex ctwi = current;
        QString path = "";
        QStringList pathlist;

        while(ctwi.parent().isValid() == true)
        {
            path = ctwi.data(Qt::DisplayRole).toString() + "/" + path;
            pathlist << ctwi.data(Qt::DisplayRole).toString();
            ctwi = ctwi.parent();
        }

        QStandardItem *item = am->invisibleRootItem()->child(0);

        qDebug() << item << pathlist.count();
        int j = pathlist.count() - 1;
        for(int i = 0; i < item->rowCount(); i++)
        {
            qDebug() << item->child(i)->text() << pathlist.at(j);
            if(item->child(i)->text() == pathlist.at(j))
            {
                item = item->child(i);
                qDebug() << item->child(i)->text();
                if(j==0) break;
                j--;
            }
        }

        qDebug() << "item->rowCount()" << item->rowCount() << item->text();

        for(int i = 0; i < item->rowCount(); i++)
        {
            model->appendRow(new QStandardItem(item->child(i)->text()));
        }

//        int i=0;
//        qDebug() << i;
//        while(current.child(i,0).isValid())
//        {
//            qDebug() << i;
//            model->invisibleRootItem()->appendRow(new QStandardItem(current.child(i,0).data(Qt::DisplayRole).toString()));
//            i++;
//        }
//            for(int i=0; i < archive_files.count() ; i++)
//            {
//                if(archive_files.at(i).startsWith(path))
//                {
//                    qDebug() << archive_files.at(i).indexOf('/', path.count() + 1);
//                    if(archive_files.at(i).indexOf('/', path.count() + 1) == -1)
//                    {
//                        fileList->addItem(new QListWidgetItem(archive_files.at(i), 0, makeArchiveNumberForTreewidget(i)));
//                    }
//                }
//            }
    }
}

void ThumbnailViewer::setViewMode(ViewMode mode)
{
    if(mode == this->viewMode()) return;

    int iconSize = -1;
    int gridSize = -1;

    if(mode == QListView::IconMode)
    {
        folderChangedFlag = true;
        iconSize = thumbSize;
        gridSize = thumbSize + 50;
    }
    QListView::setViewMode( mode );

    this->setIconSize( QSize( iconSize, iconSize ) );
    this->setGridSize( QSize( gridSize, gridSize) );

    setCurrentDirectory(this->path, this->isZip, this->zipFileName);
}


void ThumbnailViewer::startShowingThumbnails()
{
    showThumbnail();
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
