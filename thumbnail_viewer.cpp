#include "thumbnail_viewer.h"

#include <QtCore/qdir.h>
#include <QtGui/qfileiconprovider.h>

ThumbnailViewer::ThumbnailViewer(const QStringList &filters_archive, const QStringList &filters_image, QWidget * parent)
{
    folderChangedFlag = false;
    this->filters_archive = filters_archive;
    this->filters_image = filters_image;
    this->thumbSize = 200;

    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setIconSize(QSize(thumbSize, thumbSize));
    this->setUniformItemSizes(true);
    this->setGridSize(QSize(thumbSize + 50, thumbSize + 50));
    this->setViewMode(QListView::IconMode);

    threadThumbnails = new QThread();
    pl = new PixmapLoader();
    pl->moveToThread(threadThumbnails);
    pl->setThumbnailSize(this->thumbSize);
    connect(threadThumbnails, SIGNAL(started()), pl, SLOT(loadPixmap()));
    connect(threadThumbnails, SIGNAL(finished()), this, SLOT(onThreadThumbsFinished()));
    connect(pl, SIGNAL(finished(QIcon)), this, SLOT(onThumbnailFinished(QIcon)));
}

void ThumbnailViewer::startShowingThumbnails(const QString& path, bool isZip)
{
    this->isZip = isZip;
    this->zipFileName = zipFileName;
    if(this->path != path)
    {
        folderChangedFlag = true;
        thumbCount = 0;
        this->path = path;

        if(threadThumbnails->isRunning() == false)
        {
            showThumbnails();
        }
    }
}

void ThumbnailViewer::showThumbnails()
{
    if(this->count() == 0)
    {
        return;
    }
    while(true)
    {
        if(this->item(thumbCount)->type() == TYPE_FILE || this->item(thumbCount)->type() >= makeArchiveNumberForTreewidget(0))
        {
            break;
        }

        thumbCount++;
        if(thumbCount >= this->count())
        {
            return;
        }
    }

    if(this->isZip == true)
    {
        pl->setFilePath(path, true, this->item(thumbCount)->text());
    }
    else
    {
        pl->setFilePath(path + "/" + this->item(thumbCount)->text());
    }
    threadThumbnails->start();
}

void ThumbnailViewer::onThreadThumbsFinished()
{
    if(thumbCount < this->count() - 1 && folderChangedFlag == false)
    {
        thumbCount++;
        showThumbnails();
    }
    else
    {
        thumbCount = 0;
        if(folderChangedFlag == true)
        {
            folderChangedFlag = false;
            showThumbnails();
        }
    }
}

void ThumbnailViewer::onThumbnailFinished(QIcon icon)
{
    if(!icon.isNull())
    {
        this->item(thumbCount)->setIcon(icon);
    }
    threadThumbnails->exit();
}
