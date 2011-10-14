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
    void setCurrentDirectory(const QString &filePath, bool isZip = false, const QString &zipPath = "");

private:
    void startShowingThumbnails();

    QFutureWatcher<QPixmap> *imageScaling;
    QFutureWatcher<QPixmap> *imageLoad;

    QString path;
    QString zipPath;
    int thumbSize;
    bool isZip;
    bool folderChangedFlag;

    ArchiveModel *am;
    QSortFilterProxyModel *proxy;

private slots:
    void showImage(int num);
    void imageFinished(int);
    void currentChanged ( const QModelIndex & current, const QModelIndex & previous );
    void OnTreeViewArchiveDirsCurrentChanged ( const QModelIndex & index );

signals:
    void imageLoaded(QPixmap);

};

#endif // THUMBNAILVIEWER_H
