#ifndef VIEWFILES_H
#define VIEWFILES_H

#include "helper.h"
#include "picture_loader.h"
#include "thumbnailitemdelegate.h"
#include "model_files.h"
#include "view_archive_dirs.h"
#include "settings.h"


#include <QListView>
#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QFutureWatcher>
#include <QtConcurrentMap>
#include <QSplitter>

class ViewFiles : public QWidget
{
    Q_OBJECT

public:
    ViewFiles(QWidget *parent);
    void setViewMode(int mode);
    void setCurrentFile(const FileInfo &info);
    void setThumbnailsSize(const QSize &size);
    FileInfo getCurrentFileInfo() const;
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
        void setViewMode(int mode);
    private:
        ViewFiles *m_parent;
    };

    class TreeViewFiles : public QTreeView
    {
    public:
        explicit TreeViewFiles(ViewFiles *parent);

    private:
        ViewFiles *m_parent;
    };

    void initViewItem();
    void showThumbnails();

    FilesModel *m_model_files;
    ViewArchiveDirs *m_view_archiveDirs;
    TreeViewFiles *m_treeView_files;
    ListViewFiles *m_listView_files;
    QSplitter *m_splitter;
    ThumbnailItemDelegate *m_thumbnail_delegate;

    QAbstractItemView *m_view_current;
    QAbstractItemModel *m_model_current;
    FileInfo m_fileinfo_current;

    int m_view_mode;
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
