#include "thumbnail_viewer.h"

ThumbnailViewer::ThumbnailViewer(QWidget * parent)
{
    threadThumbnails = new QThread();
    gt = new thumbnailGenerator();
    gt->setLength(200);
    gt->moveToThread(threadThumbnails);
    connect(threadThumbnails, SIGNAL(started()), gt, SLOT(returnThumbnail()));
    connect(threadThumbnails, SIGNAL(finished()), this, SLOT(onThreadThumbsFinished()));
    connect(gt, SIGNAL(finished(QIcon)), this, SLOT(onThumbnailFinished(QIcon)));
}

void ThumbnailViewer::startShowingThumbnails(const QString& path)
{
    thumbCount = 0;
    this->path = path;
    showThumbnails();
}

void ThumbnailViewer::showThumbnails()
{
    if(this->count() == 0)
    {
        return;
    }
    while(this->item(thumbCount)->type() != TYPE_FILE)
    {
        thumbCount++;
        if(thumbCount >= this->count())
        {
            return;
        }
    }

    const QString& filepath = path + this->item(thumbCount)->text();
    gt->setPath(filepath);
    threadThumbnails->start();
}

void ThumbnailViewer::onThreadThumbsFinished()
{
    if(thumbCount < this->count() - 1)
    {
        thumbCount++;
        showThumbnails();
    }
    else
    {
        thumbCount = 0;
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
