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
    void setShowThumbnails(bool b);

public slots:
    void pageNext();
    void pagePrevious();

private:

    class ListViewFiles : public QListView
    {
    public:
        explicit ListViewFiles(ViewFiles *parent);
        void startShowingThumbnails();

        void setModel(QAbstractItemModel *model);
        QModelIndex getIndexFromProxy(const QModelIndex &index) const;
        void setCurrentIndex(const QModelIndex &index);
        void setRootIndex(const QModelIndex &index);
        void setShowThumbnails(bool b);

    private:
        ViewFiles *m_parent;
        QSortFilterProxyModel *m_proxy;
        ThumbnailItemDelegate *m_thumbnail_delegate;
    };

    class TreeViewFiles : public QTreeView
    {
    public:
        explicit TreeViewFiles(ViewFiles *parent);
        void startShowingThumbnails();
        void setShowThumbnails(bool b);

    private:
        ViewFiles *m_parent;
        ThumbnailItemDelegate *m_thumbnail_delegate;
    };

    void initViewItem();
    void showThumbnails();
    TreeViewFiles *m_treeView_files;
    ListViewFiles *m_listView_files;
    QAbstractItemView *m_aiv;
    QModelIndex m_index_current_archive_dirs;

    QAbstractItemModel *m_model;
    FileInfo m_currentInfo;
    QListView::ViewMode m_mode;
    QSize m_thumb_size;
    bool m_show_thumbnails;

private slots:
    void on_item_activated(const QModelIndex &index);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_archiveDirsView_currentRowChanged(const QModelIndex &index);
    void on_thumbnail_finished(const QModelIndex &index);

signals:
    void currentFileChanged(const FileInfo &info);
    void activated(const QModelIndex &index);

};

#endif // VIEWFILES_H
