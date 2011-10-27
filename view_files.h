#ifndef VIEWFILES_H
#define VIEWFILES_H

#include "komicviewer_enums.h"
#include "picture_loader.h"
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
    ViewFiles(QWidget *parent = 0);
    void setModel(QAbstractItemModel *model);
    void setViewMode(ViewMode mode);
    void setCurrentDirectory(const FileInfo &info);
    QModelIndex getIndexFromProxy(const QModelIndex &index);
    FileInfo getCurrentFileInfo();

public slots:
    void pageNext();
    void pagePrevious();

private:
    void startShowingThumbnails();

    int returnThumbCount;
    QFutureWatcher<QPixmap> *watcherThumbnail;
    FileInfo currentInfo;
    QSortFilterProxyModel *proxy;

private slots:
    void showThumbnail(int num);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void OnTreeViewArchiveDirsCurrentChanged(const QModelIndex &index);

signals:
    void currentFileChanged(FileInfo);

};

#endif // VIEWFILES_H
