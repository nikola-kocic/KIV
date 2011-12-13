#ifndef VIEWFILES_H
#define VIEWFILES_H

#include "helper.h"
#include "picture_loader.h"

#include <QListView>
#include <QSortFilterProxyModel>
#include <QFutureWatcher>
#include <QtConcurrentMap>

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

    int m_returnThumbCount;
    QFutureWatcher<QImage> *m_watcherThumbnail;
    FileInfo m_currentInfo;
    QSortFilterProxyModel *m_proxy;

private slots:
    void showThumbnail(int num);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_archiveDirsView_currentRowChanged(const QModelIndex &index);

signals:
    void currentFileChanged(FileInfo);

};

#endif // VIEWFILES_H
