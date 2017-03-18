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
#include <QtConcurrent/QtConcurrentMap>
#include <QTreeView>
#include <QUrl>

#include <functional>

#include "include/IArchiveExtractor.h"
#include "helper.h"
#include "models/archive_model.h"
#include "models/filesystem_model.h"
#include "models/modelwrapper.h"
#include "settings.h"
#include "widgets/files_view/sortcombobox.h"
#include "widgets/files_view/thumbnailitemdelegate.h"
#include "widgets/files_view/view_archive_dirs.h"
#include "widgets/files_view/view_files_list.h"
#include "widgets/files_view/view_files_tree.h"
#include "widgets/files_view/ifileview.h"

class ViewFiles : public QWidget
{
    Q_OBJECT

public:
    explicit ViewFiles(ThumbnailItemDelegate * const thumbnail_item_delegate,
                       const IArchiveExtractor* const archive_extractor,
                       IModelWrapper *model_filesystem,
                       QWidget *parent = nullptr);
    ~ViewFiles() override;
    void setViewMode(const FileViewMode mode);
    void setCurrentFile(const FileInfo &info);
    void setThumbnailsSize(const QSize &size);
    FileInfo getCurrentFileInfo() const;
    void setShowThumbnails(const bool b);
    void saveCurrentFile(const QString &fileName) const;
    bool changeImage(std::function<int()> generator);
    void pageNext();
    void pagePrevious();
    void setLocationUrl(const QUrl &url);
    void dirUp();

private:
    void initViewItem();
    void showThumbnails();
    IModelWrapper *createArchiveModel(const FileInfo &info);

    const IArchiveExtractor* const m_archive_extractor;

    IFileView *m_view_archiveDirs;
    QTreeView *m_view_filesystem;

    TreeViewFiles *m_treeView_files;
    ListViewFiles *m_listView_files;
    QAbstractItemView *m_view_current;

    std::shared_ptr<IModelWrapper> m_model_archive_files;
    IModelWrapper *m_model_filesystem;

    /* source model is FileSystemModel or ArchiveModel */
    FileListSortFilterProxyModel *m_proxy_file_list;

    /* source model is FileSystemModel */
    ContainersSortFilterProxyModel *m_proxy_containers;

    ThumbnailItemDelegate *const m_thumbnail_delegate;

    QVBoxLayout *m_layout_files_list;
    SortComboBox *m_combobox_sort;

    FileInfo m_fileinfo_current;
    FileViewMode m_view_mode;

    bool m_show_thumbnails;
    bool m_flag_opening;

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
