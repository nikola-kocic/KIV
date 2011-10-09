#include "thumbnail_viewer.h"

#include <QtCore/qdir.h>
#include <QtGui/qfileiconprovider.h>

ThumbnailViewer::ThumbnailViewer(QWidget * parent)
{
    this->thumbSize = 200;

    this->setResizeMode(QListView::Adjust);
    this->setMovement(QListView::Static);
    this->setIconSize(QSize(thumbSize, thumbSize));
    this->setUniformItemSizes(true);
    this->setGridSize(QSize(thumbSize + 50, thumbSize + 50));
    this->setViewMode(QListView::IconMode);

    threadThumbnails = new QThread();
    gt = new ThumbnailGenerator();
    gt->setLength(thumbSize);
    gt->moveToThread(threadThumbnails);
    connect(threadThumbnails, SIGNAL(started()), gt, SLOT(returnThumbnail()));
    connect(threadThumbnails, SIGNAL(finished()), this, SLOT(onThreadThumbsFinished()));
    connect(gt, SIGNAL(finished(QIcon)), this, SLOT(onThumbnailFinished(QIcon)));
}

void ThumbnailViewer::startShowingThumbnails(const QString& path, const QStringList &filters_archive, const QStringList &filters_image)
{
    thumbCount = 0;
    this->path = path;

    this->clear();

    QDir dir(this->path);

    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
    QFileIconProvider fip;
    for (int i=0; i < list.count(); i++)
    {
        QListWidgetItem* nlvfi = NULL;

        QFileInfo info = list.at(i);
        if(info.isDir())
        {
            nlvfi = new QListWidgetItem(info.fileName(), 0, TYPE_DIR);
        }
        else if(filters_archive.contains(info.suffix().toLower()))
        {
            nlvfi = new QListWidgetItem(info.fileName(), 0, TYPE_ARCHIVE);
        }
        else if (filters_image.contains(info.suffix().toLower()))
        {
            nlvfi = new QListWidgetItem(info.fileName(), 0, TYPE_FILE);
        }

        if(nlvfi != NULL)
        {
            nlvfi->setIcon(fip.icon(info));
            nlvfi->setSizeHint(QSize(thumbSize + 20, thumbSize + 20));

            nlvfi->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
            nlvfi->setToolTip(info.absoluteFilePath());

            this->addItem(nlvfi);
        }
    }

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

    gt->setPath(path + "/" + this->item(thumbCount)->text());
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
