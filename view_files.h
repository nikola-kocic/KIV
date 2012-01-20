#ifndef VIEWFILES_H
#define VIEWFILES_H

#include "helper.h"
#include "picture_loader.h"
#include "thumbnailitemdelegate.h"
#include "view_archive_dirs.h"
#include "settings.h"
#include "archive_model.h"

#include <QListView>
#include <QTreeView>
#include <QFutureWatcher>
#include <QtConcurrentMap>
#include <QSplitter>
#include <QFileSystemModel>
#include <QPointer>

class ListViewFiles;
class TreeViewFiles;
class FileSystemModel;

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:
    explicit FileSystemModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

class ListViewFiles : public QListView
{
    Q_OBJECT

public:
    explicit ListViewFiles(QWidget *parent = 0);

    void setViewMode(const int mode);

protected slots:
    void rowsInserted(const QModelIndex &parent, int start, int end);

signals:
    void rowsInserted(const QModelIndexList &indexes);
};


class TreeViewFiles : public QTreeView
{
    Q_OBJECT

public:
    explicit TreeViewFiles(QWidget *parent = 0);

protected slots:
    void rowsInserted(const QModelIndex &parent, int start, int end);

signals:
    void rowsInserted(const QModelIndexList &indexes);
};


class ViewFiles : public QWidget
{
    Q_OBJECT

public:
    explicit ViewFiles(QWidget *parent = 0);
    void setViewMode(const int mode);
    void setCurrentFile(const FileInfo &info);
    void setThumbnailsSize(const QSize &size);
    FileInfo getCurrentFileInfo() const;
    void setModel(QAbstractItemModel *model);
    void setShowThumbnails(const bool b);

public slots:
    void pageNext();
    void pagePrevious();

private:
    class MySortFilterProxyModel : public QSortFilterProxyModel
    {
    public:
        explicit MySortFilterProxyModel(QObject *parent = 0);
    protected:
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    };
    void initViewItem();
    void showThumbnails();

    FileInfo m_fileinfo_current;
    int m_view_mode;
    bool m_show_thumbnails;
    QSize m_thumb_size;

    ViewArchiveDirs *m_view_archiveDirs;

    TreeViewFiles *m_treeView_files;
    ListViewFiles *m_listView_files;
    QAbstractItemView *m_view_current;

    QPointer<ArchiveModel> m_model_archive_files;
    FileSystemModel *m_model_filesystem;
    MySortFilterProxyModel *m_proxy_sort;

    QSplitter *m_splitter;
    ThumbnailItemDelegate *m_thumbnail_delegate;


private slots:
    void on_item_activated(const QModelIndex &index);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_archiveDirsView_currentRowChanged(const QModelIndex &index);
    void on_thumbnail_finished(const QModelIndex &index);
    void on_rows_inserted(const QModelIndexList &indexes);

signals:
    void currentFileChanged(const FileInfo &info);
    void activated(const QString &path);
};

#endif // VIEWFILES_H
