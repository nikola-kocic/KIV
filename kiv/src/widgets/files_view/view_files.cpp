#include "kiv/src/widgets/files_view/view_files.h"

#include <QHeaderView>
#include <QLabel>

#include "kiv/src/models/archive_item.h"
#include "kiv/src/picture_loader.h"

//#define DEBUG_VIEW_FILES
#ifdef DEBUG_VIEW_FILES
#include "kiv/src/helper.h"
#endif

ViewFiles::ViewFiles(ThumbnailItemDelegate *const thumbnail_item_delegate,
                     std::unique_ptr<const IArchiveExtractor> archive_extractor,
                     IModelWrapper *model_filesystem,
                     QWidget *parent)
    : QWidget(parent)

    , m_archive_extractor(std::move(archive_extractor))
    , m_fileinfo_current(FileInfo(""))
    , m_view_mode(FileViewMode::List)
    , m_show_thumbnails(false)
    , m_flag_opening(false)

    , m_view_archiveDirs(new ViewArchiveDirs(
                             std::unique_ptr<QTreeView>(new QTreeView())
                             , std::unique_ptr<QAbstractProxyModel>(
                                 new ArchiveDirsSortFilterProxyModel())
                             , this
                             ))
    , m_view_filesystem(new QTreeView(this))
    , m_treeView_files(nullptr)
    , m_listView_files(nullptr)
    , m_view_current(nullptr)

    , m_model_archive_files(nullptr)
    , m_model_filesystem(model_filesystem)
    , m_proxy_file_list(new FileListSortFilterProxyModel(this))
    , m_proxy_containers(new ContainersSortFilterProxyModel(this))

    , m_thumbnail_delegate(thumbnail_item_delegate)
    , m_layout_files_list(new QVBoxLayout())
    , m_combobox_sort(new SortComboBox(
                          QList<ColumnSort>()
                          << ColumnSort(Column::Name, SortOrder::Asc)
                          << ColumnSort(Column::Name, SortOrder::Desc)
                          << ColumnSort(Column::Date, SortOrder::Asc)
                          << ColumnSort(Column::Date, SortOrder::Desc)
                          << ColumnSort(Column::Size, SortOrder::Asc)
                          << ColumnSort(Column::Size, SortOrder::Desc)
                          , this
                          ))
{
    m_proxy_containers->setSourceModel(m_model_filesystem->getModel());

    connect(m_thumbnail_delegate, SIGNAL(thumbnailFinished(QModelIndex)),
            this, SLOT(on_thumbnail_finished(QModelIndex)));

    /* Start archiveDirsView */
    m_view_archiveDirs->getWidget()->hide();
    /* End archiveDirsView */

    /* Start filesystemView */
    m_view_filesystem->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view_filesystem->setUniformRowHeights(true);
    m_view_filesystem->setHeaderHidden(true);
    m_view_filesystem->setModel(m_proxy_containers);
    /* End filesystemView */


    m_combobox_sort->setSort(ColumnSort(Column::Name, SortOrder::Asc));


    /* Start Layout */

    QHBoxLayout *layout_sort = new QHBoxLayout();
    layout_sort->setSpacing(5);
    QLabel *label_sort = new QLabel(tr("Sort:"), this);
    label_sort->setIndent(5);
    layout_sort->addWidget(label_sort);
    layout_sort->addWidget(m_combobox_sort, 1);

    m_layout_files_list->setMargin(0);
    m_layout_files_list->setSpacing(0);
    m_layout_files_list->addLayout(layout_sort);

    QWidget *widget_files_list = new QWidget(this);
    widget_files_list->setLayout(m_layout_files_list);

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    policy.setHorizontalStretch(0);
    policy.setVerticalStretch(1);
    widget_files_list->setSizePolicy(policy);


    QSplitter *m_splitter = new QSplitter(Qt::Vertical, this);
    m_splitter->addWidget(m_view_archiveDirs->getWidget());
    m_splitter->setSizes(QList<int>() << 100);
    m_splitter->addWidget(widget_files_list);

    QSplitter *m_splitter_sidebar = new QSplitter(Qt::Vertical, this);

    m_splitter_sidebar->addWidget(m_view_filesystem);
    m_splitter_sidebar->addWidget(m_splitter);
    m_splitter_sidebar->setSizes(QList<int>() << 400 << 400);

    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setSpacing(0);
    layoutMain->setMargin(0);
    layoutMain->addWidget(m_splitter_sidebar);
    this->setLayout(layoutMain);

    /* End Layout */

    connect(m_view_archiveDirs,
            SIGNAL(currentRowChanged(QModelIndex)),
            this,
            SLOT(on_archiveDirsView_currentRowChanged(QModelIndex)));
    connect(m_view_filesystem->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(on_filesystemView_currentRowChanged(QModelIndex,QModelIndex)));
    connect(m_combobox_sort, SIGNAL(currentSortChanged(ColumnSort)),
            this, SLOT(on_combobox_sort_currentSortChanged(ColumnSort)));

    initViewItem();
}

ViewFiles::~ViewFiles()
{
}

FileInfo ViewFiles::getCurrentFileInfo() const
{
    return m_fileinfo_current;
}


void ViewFiles::initViewItem()
{
#ifdef DEBUG_VIEW_FILES
    DEBUGOUT << m_view_mode;
#endif
    if (m_view_mode == FileViewMode::Details)
    {
        m_treeView_files = new TreeViewFiles(this);
        m_listView_files = nullptr;
        m_view_current = qobject_cast<QAbstractItemView *>(m_treeView_files);
        connect(m_treeView_files, SIGNAL(rowsInserted(QModelIndexList)),
                this, SLOT(on_rows_inserted(QModelIndexList)));
    }
    else
    {
        m_listView_files = new ListViewFiles(this);
        m_listView_files->setViewMode(m_view_mode);
        m_treeView_files = nullptr;
        m_view_current = qobject_cast<QAbstractItemView *>(m_listView_files);
        connect(m_listView_files, SIGNAL(rowsInserted(QModelIndexList)),
                this, SLOT(on_rows_inserted(QModelIndexList)));
    }

    m_view_current->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view_current->setModel(m_proxy_file_list);
    connect(m_view_current->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(on_FilesView_currentRowChanged(QModelIndex,QModelIndex)));
    connect(m_view_current, SIGNAL(activated(QModelIndex)),
            this, SLOT(on_item_activated(QModelIndex)));

    if (m_fileinfo_current.isValid())
    {
        setCurrentFile(m_fileinfo_current);
    }
    m_layout_files_list->addWidget(m_view_current);

    QSizePolicy policyV(QSizePolicy::Preferred, QSizePolicy::Expanding);
    policyV.setHorizontalStretch(0);
    policyV.setVerticalStretch(1);
    m_view_current->setSizePolicy(policyV);

    setShowThumbnails(m_show_thumbnails);
}

void ViewFiles::setViewMode(const FileViewMode mode)
{
#ifdef DEBUG_VIEW_FILES
    DEBUGOUT << mode;
#endif
    if (m_view_mode != mode)
    {
        /* Check if View from previous View Mode can be reused */
        if ((mode == FileViewMode::Icons || mode == FileViewMode::List)
            && (m_view_mode == FileViewMode::Icons
                || m_view_mode == FileViewMode::List)
            )
        {
            m_listView_files->setViewMode(mode);
            m_view_mode = mode;
        }
        else
        {
            m_view_current->setModel(nullptr);
            m_view_current->disconnect();
            m_view_current->deleteLater();
            m_view_mode = mode;
            initViewItem();
        }
    }
}

void ViewFiles::setLocationUrl(const QUrl &url)
{
    const FileInfo fileinfo(url.toLocalFile());
    return setCurrentFile(fileinfo);
}

IModelWrapper *ViewFiles::createArchiveModel(const FileInfo &info)
{
    const QString archive_path = info.getContainerPath();
    std::vector<ArchiveFileInfo> archive_files;
    const int success = m_archive_extractor->getFileInfoList(archive_path, archive_files);
    Q_UNUSED(success);
    return new ArchiveModelWrapper(new ArchiveModel(archive_files, archive_path));
}

void ViewFiles::setCurrentFile(const FileInfo &info)
{
#ifdef DEBUG_VIEW_FILES
    DEBUGOUT << info.getDebugInfo();
#endif

    m_flag_opening = true;

    m_view_filesystem->setCurrentIndex(
                m_proxy_containers->mapFromSource(
                    m_model_filesystem->getContainer(info)));

    if (info.isInArchive())
    {
        m_proxy_file_list->setSourceModel(nullptr);
        m_view_archiveDirs->setModel(nullptr);

        m_model_archive_files = std::shared_ptr<IModelWrapper>(createArchiveModel(info));

        m_proxy_file_list->setSourceModel(m_model_archive_files->getModel());
        m_view_archiveDirs->setModel(m_model_archive_files->getModel());

        if (!m_fileinfo_current.isInArchive() || !m_fileinfo_current.isValid())
        {
            m_view_archiveDirs->getWidget()->show();
        }
        const QModelIndex dirIndex = m_model_archive_files->getContainer(info);
        m_view_archiveDirs->setCurrentIndex(dirIndex);

        if (info.fileExists())
        {
            m_view_current->setCurrentIndex(
                        m_proxy_file_list->mapFromSource(
                            m_model_archive_files->getFile(info, dirIndex)));
        }
    }
    else
    {
        if (m_fileinfo_current.isInArchive() || !m_fileinfo_current.isValid())
        {
            m_proxy_file_list->setSourceModel(nullptr);
            m_view_archiveDirs->setModel(nullptr);
            m_view_archiveDirs->getWidget()->hide();

            m_proxy_file_list->setSourceModel(m_model_filesystem->getModel());
        }
        const QModelIndex dirIndex = m_model_filesystem->getContainer(info);
        m_view_current->setRootIndex(
                    m_proxy_file_list->mapFromSource(dirIndex));
        m_view_current->setCurrentIndex(
                    m_proxy_file_list->mapFromSource(
                        m_model_filesystem->getFile(info, dirIndex)));
    }

    m_view_current->scrollTo(m_view_current->currentIndex());

#ifdef DEBUG_VIEW_FILES
    DEBUGOUT << "end" << info.getDebugInfo();
#endif
    m_fileinfo_current = info;
    m_flag_opening = false;

    showThumbnails();
}

void ViewFiles::on_filesystemView_currentRowChanged(
        const QModelIndex &current, const QModelIndex &/*previous*/)
{
    const QModelIndex index0 = m_proxy_containers->mapToSource(current);
    if (m_model_filesystem->isDir(index0))
    {
        m_model_filesystem->getModel()->fetchMore(index0);
    }
    m_view_filesystem->scrollTo(current);

    if (m_flag_opening)
    {
        return;
    }

    QString currentContainer = m_model_filesystem->filePath(index0);
    emit urlChanged(QUrl::fromLocalFile(currentContainer));

    const FileInfo info = FileInfo(currentContainer);
#ifdef DEBUG_MAIN_WINDOW
    DEBUGOUT << info.getDebugInfo();
#endif

    this->setCurrentFile(info);
}

void ViewFiles::on_combobox_sort_currentSortChanged(ColumnSort sort)
{
    Qt::SortOrder sortOrder = sort.getOrder() == SortOrder::Asc ?
                Qt::AscendingOrder: Qt::DescendingOrder;
    switch (sort.getColumn())
    {
    case Column::Name:
        if (m_fileinfo_current.isInArchive())
        {
            m_proxy_file_list->sort(ArchiveItem::col_name, sortOrder);
        }
        else
        {
            m_proxy_file_list->sort(0, sortOrder);
        }
        break;
    case Column::Date:
        if (m_fileinfo_current.isInArchive())
        {
            m_proxy_file_list->sort(ArchiveItem::col_date, sortOrder);
        }
        else
        {
            m_proxy_file_list->sort(3, sortOrder);
        }
        break;
    case Column::Size:
        if (m_fileinfo_current.isInArchive())
        {
            m_proxy_file_list->sort(ArchiveItem::col_size, sortOrder);
        }
        else
        {
            m_proxy_file_list->sort(1, sortOrder);
        }
        break;
    }
}

void ViewFiles::dirUp()
{
    if (m_fileinfo_current.isInArchive()
        && m_view_archiveDirs->currentIndex().parent().isValid())
    {
        m_view_archiveDirs->setCurrentIndex(
                    m_view_archiveDirs->currentIndex().parent());
    }
    else if (m_view_filesystem->currentIndex().parent().isValid())
    {
        m_view_filesystem->setCurrentIndex(
                    m_view_filesystem->currentIndex().parent());
    }
}

void ViewFiles::on_archiveDirsView_currentRowChanged(const QModelIndex &current)
{
    // Indexes are from ArchiveDirsSortFilterProxyModel
#ifdef DEBUG_VIEW_FILES
    DEBUGOUT << m_fileinfo_current.getDebugInfo();
#endif

    if (!current.isValid())
    {
        return;
    }

    m_view_current->setRootIndex(m_proxy_file_list->mapFromSource(current));
    m_view_current->selectionModel()->clear();
    m_view_current->scrollToTop();
    showThumbnails();

    if (m_flag_opening)
    {
        return;
    }
    const QString filePath = m_model_archive_files->filePath(current);
    m_fileinfo_current = FileInfo(filePath);
    emit urlChanged(QUrl::fromLocalFile(filePath));
}

void ViewFiles::on_FilesView_currentRowChanged(const QModelIndex &current,
                                               const QModelIndex &/*previous*/)
{
    if (!current.isValid())
    {
        return;
    }

    m_view_current->scrollTo(current);

    if (m_flag_opening)
    {
        return;
    }

#ifdef DEBUG_VIEW_FILES
    DEBUGOUT << m_fileinfo_current.getDebugInfo();
#endif

    // Pass 'false' for 'IsContainer' because item is only selected
    const QString filePath =
            current.data(QFileSystemModel::FilePathRole).toString();
    m_fileinfo_current = FileInfo(filePath, false);
    emit urlChanged(QUrl::fromLocalFile(filePath));
}

void ViewFiles::pageNext()
{
    if (!m_view_current->currentIndex().isValid())
    {
        return;
    }

    IModelWrapper *m = (m_fileinfo_current.isInArchive() ? m_model_archive_files.get() : m_model_filesystem);
    for (int i = m_view_current->currentIndex().row() + 1;
         i < m_view_current->model()->rowCount(m_view_current->rootIndex());
         ++i)
    {
        const QModelIndex index = m_view_current->model()->index(
                    i, 0, m_view_current->rootIndex());
        const QModelIndex source_index = m_proxy_file_list->mapToSource(index);
        if (m->getNodeType(source_index) == NodeType::Image)
        {
            m_view_current->setCurrentIndex(index);
            break;
        }
    }
}

void ViewFiles::pagePrevious()
{
    if (!m_view_current->currentIndex().isValid())
    {
        return;
    }

    IModelWrapper *m = (m_fileinfo_current.isInArchive() ? m_model_archive_files.get() : m_model_filesystem);
    for (int i = m_view_current->currentIndex().row() - 1; i >= 0; --i)
    {
        const QModelIndex index = m_view_current->model()->index(
                    i, 0, m_view_current->rootIndex());
        const QModelIndex source_index = m_proxy_file_list->mapToSource(index);
        if (m->getNodeType(source_index) == NodeType::Image)
        {
            m_view_current->setCurrentIndex(index);
            break;
        }
    }
}


void ViewFiles::on_item_activated(const QModelIndex &index)
{
#ifdef DEBUG_VIEW_FILES
    const QString name = index.data().toString();
    DEBUGOUT<< index.internalId() << name;
#endif
    // Index is from FileListSortFilterProxyModel

    // TODO see if fixed_index is necessary
    const QModelIndex fixed_index = (index.column() > 0
                                     ? index.sibling(index.row(), 0) : index);
    const QModelIndex source_index = m_proxy_file_list->mapToSource(fixed_index);

    if (m_fileinfo_current.isInArchive())
    {
        if (m_model_archive_files->isDir(source_index))
        {
            m_view_archiveDirs->setCurrentIndex(
                            m_proxy_file_list->mapToSource(fixed_index));
        }
    }
    else
    {
        if (m_model_filesystem->isDir(source_index)
            || Helper::isArchiveFile(m_model_filesystem->fileInfo(
                                         source_index))
            )
        {
            m_view_filesystem->setCurrentIndex(
                        m_proxy_containers->mapFromSource(source_index));
            m_view_filesystem->expand(m_view_filesystem->currentIndex());
        }
    }
}

/* Start Thumbnails */

void ViewFiles::setThumbnailsSize(const QSize &size)
{
    if (m_thumbnail_delegate->setThumbnailSize(size))
    {
        showThumbnails();
    }
}

void ViewFiles::showThumbnails()
{

#ifdef DEBUG_VIEW_FILES
    DEBUGOUT << m_show_thumbnails << m_fileinfo_current.getDebugInfo();
#endif
    if (m_show_thumbnails)
    {
        m_thumbnail_delegate->cancelThumbnailGeneration();
        if (m_view_current->model()->rowCount(m_view_current->rootIndex()) == 0)
        {
            return;
        }

        QModelIndexList indexes;
        for (int i = 0;
             i < m_view_current->model()->rowCount(m_view_current->rootIndex());
             ++i)
        {
            const QModelIndex currIndex = m_view_current->model()->index(
                        i, 0, m_view_current->rootIndex());
            indexes.append(currIndex);
#ifdef DEBUG_VIEW_FILES
            DEBUGOUT << "\t" << currIndex.data().toString();
#endif
        }

        on_rows_inserted(indexes);
    }
}


void ViewFiles::on_rows_inserted(const QModelIndexList &indexes)
{

#ifdef DEBUG_VIEW_FILES
    DEBUGOUT << "thumbnails?" << m_show_thumbnails;
#endif
    if (!m_show_thumbnails)
    {
        return;
    }

    for (const QModelIndex &index : indexes)
    {
        const QModelIndex fixed_index = m_proxy_file_list->mapToSource(index);
        m_thumbnail_delegate->updateThumbnail(
                    FileInfo(
                        fixed_index.data(QFileSystemModel::FilePathRole).
                        toString()),
                    fixed_index);
    }
}

void ViewFiles::setShowThumbnails(const bool b)
{
#ifdef DEBUG_VIEW_FILES
    DEBUGOUT << b;
#endif
    m_show_thumbnails = b;

    if (b)
    {
        m_view_current->setItemDelegateForColumn(0, m_thumbnail_delegate);
        showThumbnails();
    }
    else
    {
        m_view_current->setItemDelegateForColumn(0, 0);
    }
    m_view_current->doItemsLayout();
}

void ViewFiles::saveCurrentFile(const QString &fileName) const
{
    // TODO: Research error handling
    if (m_fileinfo_current.isInArchive())
    {
        m_archive_extractor->extract(m_fileinfo_current.getContainerPath(),
                                     m_fileinfo_current.getArchiveImagePath(),
                                     fileName);
    }
    else
    {
        QFile::copy(m_fileinfo_current.getPath(), fileName);
    }

    return;
}

void ViewFiles::on_thumbnail_finished(const QModelIndex &index)
{
#ifdef DEBUG_VIEW_FILES
    DEBUGOUT << index.internalId() << index.data().toString();
#endif

    const QModelIndex fixed_index = m_proxy_file_list->mapFromSource(index);
    m_view_current->update(fixed_index);
    m_view_current->updateGeometry();
    m_view_current->doItemsLayout();
}

/* End Thumbnails */
