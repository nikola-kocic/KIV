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

void ThumbnailViewer::startShowingThumbnails(const QString& path)
{
    if(this->path != path)
    {
        folderChangedFlag = true;
        thumbCount = 0;
        this->path = path;

        if(threadThumbnails->isRunning() == false)
        {
            populateList();
        }
    }
}

void ThumbnailViewer::populateList()
{
    this->clear();
    folderChangedFlag = false;

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

    timer.start();
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

    pl->setFilePath(path + "/" + this->item(thumbCount)->text());
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
        qDebug("%d", timer.elapsed());

        thumbCount = 0;
        if(folderChangedFlag == true)
        {
            populateList();
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
