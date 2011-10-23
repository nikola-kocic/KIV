#ifndef VIEWFILES_H
#define VIEWFILES_H

#include "komicviewer_enums.h"
#include "pixmap_loader.h"
#include "archive_model.h"

#include <QtGui/qlistview.h>
#include <QtCore/qthread.h>
#include <QtGui/qsortfilterproxymodel.h>
#include <QtCore/qtconcurrentrun.h>
#include <QtCore/qfuturewatcher.h>
#include <QtCore/qtconcurrentmap.h>

class ViewFiles : public QListView
{
    Q_OBJECT

public:
    ViewFiles(ArchiveModel *am, QWidget *parent = 0);
    void setModel(QAbstractItemModel * model);
    void setViewMode(ViewMode mode);
    void setCurrentDirectory(const FileInfo &info);
    QModelIndex getIndexFromProxy(const QModelIndex & index);

public slots:
    void pageNext();
    void pagePrevious();

private:
    void startShowingThumbnails();

    QFutureWatcher<QPixmap> *imageScaling;

    FileInfo currentInfo;
    bool folderChangedFlag;

    ArchiveModel *am;
    QSortFilterProxyModel *proxy;

private slots:
    void showImage(int num);
    void currentChanged(const QModelIndex & current, const QModelIndex & previous);
    void OnTreeViewArchiveDirsCurrentChanged(const QModelIndex & index);

signals:
    void currentFileChanged(FileInfo);

};

#endif // VIEWFILES_H
