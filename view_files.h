#ifndef VIEWFILES_H
#define VIEWFILES_H

#include "helper.h"
#include "picture_loader.h"
#include "thumbnailitemdelegate.h"
#include "settings.h"
#include "archive_model.h"

#include <QLayout>
#include <QListView>
#include <QTreeView>
#include <QFutureWatcher>
#include <QtConcurrentMap>
#include <QSplitter>
#include <QFileSystemModel>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QComboBox>

class ListViewFiles;
class TreeViewFiles;

class SortDirection
{
public:
    static const int NameAsc = 0;
    static const int NameDesc = 1;
    static const int DateAsc = 2;
    static const int DateDesc = 3;
    static const int SizeAsc = 4;
    static const int SizeDesc = 5;
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
    QFileSystemModel* getFilesystemModel() const;

public slots:
    void pageNext();
    void pagePrevious();

private:
    class FileSystemModel : public QFileSystemModel
    {
    public:
        explicit FileSystemModel(QObject *parent = 0) : QFileSystemModel(parent) {}
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    };

    class FileListSortFilterProxyModel : public QSortFilterProxyModel
    {
    public:
        explicit FileListSortFilterProxyModel(QObject *parent = 0) : QSortFilterProxyModel(parent) {}
    protected:
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    };

    class ContainersSortFilterProxyModel : public QSortFilterProxyModel
    {
    public:
        explicit ContainersSortFilterProxyModel(QObject *parent = 0) : QSortFilterProxyModel(parent) {}
    protected:
        bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
        bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    };

    class ArchiveDirsSortFilterProxyModel : public QSortFilterProxyModel
    {
    public:
        explicit ArchiveDirsSortFilterProxyModel(QObject *parent = 0) : QSortFilterProxyModel(parent) {}

    protected:
        bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    };

    void initViewItem();
    void showThumbnails();

    FileInfo m_fileinfo_current;
    int m_view_mode;
    bool m_show_thumbnails;
    bool m_flag_opening;
    QSize m_thumb_size;

    QTreeView *m_view_archiveDirs;
    QTreeView *m_view_filesystem;

    TreeViewFiles *m_treeView_files;
    ListViewFiles *m_listView_files;
    QAbstractItemView *m_view_current;

    QPointer<ArchiveModel> m_model_archive_files;
    FileSystemModel *m_model_filesystem;

    FileListSortFilterProxyModel *m_proxy_file_list; // source model is FileSystemModel or ArchiveModel
    ContainersSortFilterProxyModel *m_proxy_containers; // source model is FileSystemModel
    ArchiveDirsSortFilterProxyModel *m_proxy_archive_dirs; // source model is ArchiveModel

    ThumbnailItemDelegate *m_thumbnail_delegate;

    QVBoxLayout *m_layout_files_list;
    QComboBox *m_combobox_sort;

public slots:
    void dirUp();

private slots:
    void on_item_activated(const QModelIndex &index);
    void on_FilesView_currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_thumbnail_finished(const QModelIndex &index);
    void on_rows_inserted(const QModelIndexList &indexes);

    void on_archiveDirsView_currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_filesystemView_currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_combobox_sort_currentIndexChanged(int index);

signals:
    void currentFileChanged(const FileInfo &info);
};

#endif // VIEWFILES_H
