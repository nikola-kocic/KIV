#ifndef THUMBNAILVIEWER_H
#define THUMBNAILVIEWER_H

#include "komicviewer_enums.h"
#include "pixmap_loader.h"
#include "archive_model.h"

#include <QtGui/qlistview.h>
#include <QtCore/qthread.h>
#include <QtGui/qsortfilterproxymodel.h>
#include <QtCore/qtconcurrentrun.h>
#include <QtCore/qfuturewatcher.h>
#include <QtCore/qtconcurrentmap.h>
#include <QtCore/QTime>

class ThumbnailViewer : public QListView
{
    Q_OBJECT

public:
    ThumbnailViewer(ArchiveModel *am, QWidget *parent = 0);
    void setModel ( QAbstractItemModel * model );
    void setViewMode(ViewMode mode);
    bool folderChangedFlag;
    void setCurrentDirectory(const QString &filePath, bool isZip = false, const QString &zipFileName = "");

private:
    void populateList();
    void showThumbnail();
//    QThread *threadThumbnails;
    QFutureWatcher<QPixmap> *imageScaling;

    QString path;
    bool isZip;
    ArchiveModel *am;
    QString zipFileName;
    int thumbSize;
    void startShowingThumbnails();
    QSortFilterProxyModel *proxy;
    QTime t;

private slots:
//    void onThreadThumbsFinished();
    void showImage(int num);
//    void onThumbnailFinished(QPixmap);

    void finished();
    void OnTreeViewArchiveDirsCurrentChanged ( const QModelIndex & index );

};

#endif // THUMBNAILVIEWER_H
