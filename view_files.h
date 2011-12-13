#ifndef VIEWFILES_H
#define VIEWFILES_H

#include "helper.h"
#include "picture_loader.h"

#include <QtGui/qlistview.h>
#include <QtGui/qsortfilterproxymodel.h>
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
    QModelIndex getIndexFromProxy(const QModelIndex &index) const;
    void setCurrentIndexFromSource(const QModelIndex &index);
    FileInfo getCurrentFileInfo() const;

public slots:
    void pageNext();
    void pagePrevious();

private:
    void startShowingThumbnails();

    int returnThumbCount;
    QFutureWatcher<QImage> *watcherThumbnail;
    FileInfo currentInfo;
    QSortFilterProxyModel *proxy;

private slots:
    void showThumbnail(int num);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_archiveDirsView_currentRowChanged(const QModelIndex &index);

signals:
    void currentFileChanged(FileInfo);

};

#endif // VIEWFILES_H
