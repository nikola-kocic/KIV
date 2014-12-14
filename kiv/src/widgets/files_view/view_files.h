#ifndef VIEWFILES_H
#define VIEWFILES_H

#include <QComboBox>
#include <QFileSystemModel>
#include <QFutureWatcher>
#include <QLayout>
#include <QListView>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QSplitter>
#ifdef QT5
#include <QtConcurrent/QtConcurrentMap>
#else
#include <QtConcurrentMap>
#endif
#include <QTreeView>
#include <QUrl>

#include "kiv/include/IArchiveExtractor.h"
#include "kiv/src/helper.h"
#include "kiv/src/models/archive_model.h"
#include "kiv/src/models/filesystem_model.h"
#include "kiv/src/models/modelwrapper.h"
#include "kiv/src/settings.h"
#include "kiv/src/widgets/files_view/sortcombobox.h"
#include "kiv/src/widgets/files_view/thumbnailitemdelegate.h"
#include "kiv/src/widgets/files_view/view_archive_dirs.h"
#include "kiv/src/widgets/files_view/view_files_list.h"
#include "kiv/src/widgets/files_view/view_files_tree.h"
#include "kiv/src/widgets/files_view/ifileview.h"

class ViewFiles : public QWidget
{
    Q_OBJECT

public:
    explicit ViewFiles(const IPictureLoader *const picture_loader,
                       const IArchiveExtractor *const archive_extractor,
                       FileSystemModel *model_filesystem,
                       QWidget *parent = nullptr);
    ~ViewFiles() override;
    void setViewMode(const FileViewMode mode);
    void setCurrentFile(const FileInfo &info);
    void setThumbnailsSize(const QSize &size);
    FileInfo getCurrentFileInfo() const;
    void setModel(QAbstractItemModel *model);
    void setShowThumbnails(const bool b);
    QFileSystemModel* getFilesystemModel() const;
    void saveCurrentFile(const QString &fileName) const;

public slots:
    void pageNext();
    void pagePrevious();
    void setLocationUrl(const QUrl &url);

private:
    void initViewItem();
    void showThumbnails();

    const IPictureLoader *const m_picture_loader;
    const IArchiveExtractor *const m_archive_extractor;
    FileInfo m_fileinfo_current;
    FileViewMode m_view_mode;
    bool m_show_thumbnails;
    bool m_flag_opening;
    QSize m_thumb_size;

    IFileView *m_view_archiveDirs;
    QTreeView *m_view_filesystem;

    TreeViewFiles *m_treeView_files;
    ListViewFiles *m_listView_files;
    QAbstractItemView *m_view_current;

    QPointer<ArchiveModel> m_model_archive_files;
    FileSystemModel *m_model_filesystem;

    /* source model is FileSystemModel or ArchiveModel */
    FileListSortFilterProxyModel *m_proxy_file_list;

    /* source model is FileSystemModel */
    ContainersSortFilterProxyModel *m_proxy_containers;

    ThumbnailItemDelegate *m_thumbnail_delegate;

    QVBoxLayout *m_layout_files_list;
    SortComboBox *m_combobox_sort;

public slots:
    void dirUp();

private slots:
    void on_item_activated(const QModelIndex &index);
    void on_FilesView_currentRowChanged(const QModelIndex &current,
                                        const QModelIndex &previous);
    void on_thumbnail_finished(const QModelIndex &index);
    void on_rows_inserted(const QModelIndexList &indexes);

    void on_archiveDirsView_currentRowChanged(const QModelIndex &current);
    void on_filesystemView_currentRowChanged(const QModelIndex &current,
                                             const QModelIndex &previous);
    void on_combobox_sort_currentSortChanged(ColumnSort sort);

signals:
    void urlChanged(const QUrl &url);
};

#endif // VIEWFILES_H
