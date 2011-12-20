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
#include <QFutureWatcher>
#include <QtConcurrentMap>
#include <QSplitter>
#include <QFileSystemModel>

class ListViewFiles;
class TreeViewFiles;

class ListViewFiles : public QListView
{
    Q_OBJECT
public:
    explicit ListViewFiles(QWidget *parent);
    void setViewMode(int mode);
protected slots:
    void rowsInserted(const QModelIndex &parent, int start, int end);
signals:
    void rowsInserted(const QModelIndexList &indexes);
};

class TreeViewFiles : public QTreeView
{
public:
    explicit TreeViewFiles(QWidget *parent);
};

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
    void initViewItem();
    void showThumbnails();

    ViewArchiveDirs *m_view_archiveDirs;

    TreeViewFiles *m_treeView_files;
    ListViewFiles *m_listView_files;
    QAbstractItemView *m_view_current;

    FilesModel *m_model_archive_files;
    QFileSystemModel *m_model_filesystem;
    QAbstractItemModel *m_model_current;

    QSplitter *m_splitter;
    ThumbnailItemDelegate *m_thumbnail_delegate;

    FileInfo m_fileinfo_current;
    int m_view_mode;
    QSize m_thumb_size;
    bool m_show_thumbnails;
    bool m_in_archive;

private slots:
    void on_item_activated(const QModelIndex &index);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_archiveDirsView_currentRowChanged(const QModelIndex &index);
    void on_thumbnail_finished(const QModelIndex &index);
    void on_rows_inserted(const QModelIndexList &indexes);

signals:
    void currentFileChanged(const FileInfo &info);
};

#endif // VIEWFILES_H
