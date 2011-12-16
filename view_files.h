#ifndef VIEWFILES_H
#define VIEWFILES_H

#include "helper.h"
#include "picture_loader.h"
#include "thumbnailitemdelegate.h"

#include <QListView>
#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QFutureWatcher>
#include <QtConcurrentMap>
#include <QItemDelegate>

class ViewFiles : public QWidget
{
    Q_OBJECT

public:
    ViewFiles(QAbstractItemModel *model, QWidget *parent = 0);
    void setViewMode(QListView::ViewMode mode);
    void setCurrentDirectory(const FileInfo &info);
    void setThumbnailsSize(const QSize &size);
    FileInfo getCurrentFileInfo() const;
    void setCurrentIndex(const QModelIndex &index);
    void setRootIndex(const QModelIndex &index);
    void setModel(QAbstractItemModel *model);

public slots:
    void pageNext();
    void pagePrevious();

private:

    class ListViewFiles : public QListView
    {
    public:
        explicit ListViewFiles(ViewFiles *parent);
        void startShowingThumbnails();
        void setThumbnailsSize(const QSize &size);

        void setModel(QAbstractItemModel *model);
        QModelIndex getIndexFromProxy(const QModelIndex &index) const;
        void setCurrentIndex(const QModelIndex &index);
        void setRootIndex(const QModelIndex &index);

    private:
        ViewFiles *m_parent;
        QSortFilterProxyModel *m_proxy;
        QList <ThumbnailItemDelegate *> m_row_delegates;
    };

    void initViewItem();
    QTreeView *m_treeView_files;
    ListViewFiles *m_listView_files;
    QAbstractItemView *m_aiv;
    QModelIndex m_index_current_archive_dirs;

    QAbstractItemModel *m_model;
    FileInfo m_currentInfo;
    QListView::ViewMode m_mode;
    QSize m_thumb_size;

private slots:
    void on_thumbnail_activated(const QModelIndex &index);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_archiveDirsView_currentRowChanged(const QModelIndex &index);
    void on_thumbnail_finished(const QModelIndex &index);

signals:
    void currentFileChanged(FileInfo);
    void activated(const QModelIndex &index);

};

#endif // VIEWFILES_H
