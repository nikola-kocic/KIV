#include "view_files.h"
#include "archive_item.h"

#include <QLayout>
#include <QHeaderView>

//#define DEBUG_VIEW_FILES

#ifdef DEBUG_VIEW_FILES
#include <QDebug>
#include <QTime>
#endif

ViewFiles::ViewFiles(QWidget *parent)
    : QWidget(parent)

    , m_fileinfo_current(FileInfo(""))
    , m_view_mode(FileViewMode::List)
    , m_show_thumbnails(false)
    , m_flag_opening(false)
    , m_thumb_size(QSize(100, 100))

    , m_view_archiveDirs(new QTreeView(this))
    , m_view_filesystem(new QTreeView(this))
    , m_treeView_files(0)
    , m_listView_files(0)
    , m_view_current(0)

    , m_model_archive_files(0)
    , m_model_filesystem(new FileSystemModel(this))
    , m_proxy_file_list(new FileListSortFilterProxyModel(this))
    , m_proxy_containers(new ContainersSortFilterProxyModel(this))
    , m_proxy_archive_dirs(new ArchiveDirsSortFilterProxyModel(this))

    , m_splitter(new QSplitter(Qt::Vertical, this))
    , m_thumbnail_delegate(new ThumbnailItemDelegate(m_thumb_size, this))

{
    /* Start modelFilesystem */
    QStringList filters;
    const QStringList filtersArchive = Helper::getFiltersArchive();
    for (int i = 0; i < filtersArchive.size(); ++i)
    {
        filters.append("*." + filtersArchive.at(i));
    }
    const QStringList filtersImage = Helper::getFiltersImage();
    for (int i = 0; i < filtersImage.size(); ++i)
    {
        filters.append("*." + filtersImage.at(i));
    }

    m_model_filesystem->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    m_model_filesystem->setNameFilterDisables(false);
    m_model_filesystem->setNameFilters(filters);
    m_model_filesystem->setRootPath("");
    m_proxy_containers->setSourceModel(m_model_filesystem);
    /* End modelFilesystem */

    connect(m_thumbnail_delegate, SIGNAL(thumbnailFinished(QModelIndex)), this, SLOT(on_thumbnail_finished(QModelIndex)));

    /* Start archiveDirsView */
    m_view_archiveDirs->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view_archiveDirs->setUniformRowHeights(true);
    m_view_archiveDirs->header()->setResizeMode(QHeaderView::Stretch);
    m_view_archiveDirs->setHeaderHidden(true);

    m_view_archiveDirs->hide();

//    m_proxy_file_list->setSortRole(Qt::EditRole);
    m_view_archiveDirs->setModel(m_proxy_archive_dirs);



    /* End archiveDirsView */

    /* Start filesystemView */
    m_view_filesystem->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view_filesystem->setUniformRowHeights(true);
    m_view_filesystem->setHeaderHidden(true);
    m_view_filesystem->setModel(m_proxy_containers);
    /* End filesystemView */

    m_splitter->addWidget(m_view_archiveDirs);
    m_splitter->setSizes(QList<int>() << 100);

    QSplitter *m_splitter_sidebar = new QSplitter(Qt::Vertical, this);

    m_splitter_sidebar->addWidget(m_view_filesystem);
    m_splitter_sidebar->addWidget(m_splitter);

    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setSpacing(0);
    layoutMain->setMargin(0);
    layoutMain->addWidget(m_splitter_sidebar);
    this->setLayout(layoutMain);

    connect(m_view_archiveDirs->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), this, SLOT(on_archiveDirsView_currentRowChanged(QModelIndex, QModelIndex)));
    connect(m_view_filesystem->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(on_filesystemView_currentRowChanged(QModelIndex,QModelIndex)));

    initViewItem();
}

FileInfo ViewFiles::getCurrentFileInfo() const
{
    return m_fileinfo_current;
}


void ViewFiles::initViewItem()
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::initViewItem" << m_view_mode;
#endif
    if (m_view_mode == FileViewMode::Details)
    {
        m_treeView_files = new TreeViewFiles(this);
        m_listView_files = 0;
        m_view_current = qobject_cast<QAbstractItemView *>(m_treeView_files);
        connect(m_treeView_files, SIGNAL(rowsInserted(QModelIndexList)), this, SLOT(on_rows_inserted(QModelIndexList)));
    }
    else
    {
        m_listView_files = new ListViewFiles(this);
        m_listView_files->setViewMode(m_view_mode);
        m_treeView_files = 0;
        m_view_current = qobject_cast<QAbstractItemView *>(m_listView_files);
        connect(m_listView_files, SIGNAL(rowsInserted(QModelIndexList)), this, SLOT(on_rows_inserted(QModelIndexList)));
    }

    m_view_current->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view_current->setModel(m_proxy_file_list);
    connect(m_view_current->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex,QModelIndex)));
    connect(m_view_current, SIGNAL(activated(QModelIndex)), this, SLOT(on_item_activated(QModelIndex)));

    if (m_fileinfo_current.isValid())
    {
        setCurrentFile(m_fileinfo_current);
    }
    m_splitter->addWidget(m_view_current);

    QSizePolicy policyV(QSizePolicy::Preferred, QSizePolicy::Expanding);
    policyV.setHorizontalStretch(0);
    policyV.setVerticalStretch(1);
    m_view_current->setSizePolicy(policyV);

    setShowThumbnails(m_show_thumbnails);
}

void ViewFiles::setViewMode(const int mode)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::setViewMode" << mode;
#endif
    if (m_view_mode != mode)
    {
        /* Check if View from previous View Mode can be reused */
        if ((mode + m_view_mode) == (FileViewMode::Icons + FileViewMode::List))
        {
            m_listView_files->setViewMode(mode);
            m_view_mode = mode;
        }
        else
        {
            m_view_current->setModel(0);
            m_view_current->disconnect();
            m_view_current->deleteLater();
            m_view_mode = mode;
            initViewItem();
        }
    }
}

void ViewFiles::setCurrentFile(const FileInfo &info)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::setCurrentFile" << info.getDebugInfo();
#endif

    m_flag_opening = true;

    m_view_filesystem->setCurrentIndex(m_proxy_containers->mapFromSource(m_model_filesystem->index(info.getContainerPath())));

    if (info.isInArchive())
    {
        m_proxy_file_list->setSourceModel(0);
        m_proxy_archive_dirs->setSourceModel(0);

        delete m_model_archive_files;
        m_model_archive_files = new ArchiveModel(info.getContainerPath());

        m_proxy_file_list->setSourceModel(m_model_archive_files);
        m_proxy_archive_dirs->setSourceModel(m_model_archive_files);

        if (!m_fileinfo_current.isInArchive() || !m_fileinfo_current.isValid())
        {
            m_view_archiveDirs->show();
        }
        const QModelIndex dirIndex = m_model_archive_files->getDirectory(info.getZipPath());
        m_view_archiveDirs->setCurrentIndex(m_proxy_archive_dirs->mapFromSource(dirIndex));
        m_view_archiveDirs->expand(m_view_archiveDirs->currentIndex());

        if (info.fileExists())
        {
            m_view_current->setCurrentIndex(m_proxy_file_list->mapFromSource(m_model_archive_files->findIndexChild(info.getImageFileName(), dirIndex)));
        }
    }
    else
    {
        if (m_fileinfo_current.isInArchive() || !m_fileinfo_current.isValid())
        {
            m_proxy_file_list->setSourceModel(0);
            m_proxy_archive_dirs->setSourceModel(0);
            m_view_archiveDirs->hide();
            delete m_model_archive_files;

            m_proxy_file_list->setSourceModel(m_model_filesystem);
        }
        m_view_current->setRootIndex(m_proxy_file_list->mapFromSource(m_model_filesystem->index(info.getContainerPath())));
        m_view_current->setCurrentIndex(m_proxy_file_list->mapFromSource(m_model_filesystem->index(info.getPath())));
    }

//    m_proxy_file_list->sort(0);
    m_view_current->scrollTo(m_view_current->currentIndex());

#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::setCurrentFile end" << info.getDebugInfo();
#endif
    m_fileinfo_current = info;
    m_flag_opening = false;

    emit currentFileChanged(m_fileinfo_current);

    showThumbnails();
}

void ViewFiles::on_filesystemView_currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    const QModelIndex index0 = m_proxy_containers->mapToSource(current);
    if (m_model_filesystem->isDir(index0))
    {
        m_model_filesystem->fetchMore(index0);
    }
    m_view_filesystem->scrollTo(current);

    if (m_flag_opening)
        return;

    QString currentContainer = m_model_filesystem->filePath(index0);
    const FileInfo info = FileInfo(currentContainer);
#ifdef DEBUG_MAIN_WINDOW
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::on_filesystemView_currentRowChanged" << info.getDebugInfo();
#endif

    this->setCurrentFile(info);
}

void ViewFiles::dirUp()
{
    if (m_view_filesystem->currentIndex().parent().isValid())
    {
        m_view_filesystem->setCurrentIndex(m_view_filesystem->currentIndex().parent());
    }
}

void ViewFiles::on_archiveDirsView_currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    // Indexes are from ArchiveDirsSortFilterProxyModel
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::on_archiveDirsView_currentRowChanged" << m_fileinfo_current.getDebugInfo();
#endif

    Q_UNUSED(previous);
    if (!current.isValid())
    {
        return;
    }
    m_fileinfo_current = FileInfo(current.data(QFileSystemModel::FilePathRole).toString());

    m_view_current->setRootIndex(m_proxy_file_list->mapFromSource(m_proxy_archive_dirs->mapToSource(current)));
    m_view_current->selectionModel()->clear();
    showThumbnails();

    emit currentFileChanged(m_fileinfo_current);
}

void ViewFiles::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if (!current.isValid()) return;
    m_view_current->scrollTo(current);

    m_fileinfo_current = FileInfo(current.data(QFileSystemModel::FilePathRole).toString(), false); // Pass 'false' for 'IsContainer' because item is only selected

#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::currentChanged" << m_fileinfo_current.getDebugInfo();
#endif

    emit currentFileChanged(m_fileinfo_current);
}

void ViewFiles::pageNext()
{
    if (!m_view_current->currentIndex().isValid()) return;

    if (m_fileinfo_current.isInArchive())
    {
        for (int i = m_view_current->currentIndex().row() + 1; i < m_view_current->model()->rowCount(m_view_current->rootIndex()); ++i)
        {
            const int type = m_view_current->model()->index(i, 0, m_view_current->rootIndex()).data(Helper::ROLE_TYPE).toInt();
            if (type == Helper::TYPE_ARCHIVE_FILE)
            {
                m_view_current->setCurrentIndex(m_view_current->model()->index(i, 0, m_view_current->rootIndex()));
                break;
            }
        }
    }
    else
    {
        for (int i = m_view_current->currentIndex().row() + 1; i < m_view_current->model()->rowCount(m_view_current->rootIndex()); ++i)
        {
            const QModelIndex index = m_view_current->model()->index(i, 0, m_view_current->rootIndex());
            if (Helper::isImageFile(m_model_filesystem->fileInfo(index)))
            {
                m_view_current->setCurrentIndex(index);
                break;
            }
        }
    }
}

void ViewFiles::pagePrevious()
{
    if (!m_view_current->currentIndex().isValid()) return;

    if (m_fileinfo_current.isInArchive())
    {
        for (int i = m_view_current->currentIndex().row() - 1; i >= 0; --i)
        {
            const int type = m_view_current->model()->index(i, 0, m_view_current->rootIndex()).data(Helper::ROLE_TYPE).toInt();
            if (type == Helper::TYPE_ARCHIVE_FILE)
            {
                m_view_current->setCurrentIndex(m_view_current->model()->index(i, 0, m_view_current->rootIndex()));
                break;
            }
        }
    }
    else
    {
        for (int i = m_view_current->currentIndex().row() - 1; i >= 0; --i)
        {
            const QModelIndex index = m_view_current->model()->index(i, 0, m_view_current->rootIndex());
            if (Helper::isImageFile(m_model_filesystem->fileInfo(index)))
            {
                m_view_current->setCurrentIndex(index);
                break;
            }
        }
    }
}


void ViewFiles::on_item_activated(const QModelIndex &index)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::on_item_activated" <<index.internalId() <<  index.data().toString();
#endif
    // Index is from FileListSortFilterProxyModel

    // TODO see if fixed_index is necessary
    const QModelIndex fixed_index = (index.column() > 0 ? index.sibling(index.row(), 0) : index);

    if (m_fileinfo_current.isInArchive())
    {
        if (fixed_index.data(Helper::ROLE_TYPE).toInt() == Helper::TYPE_ARCHIVE_DIR)
        {
            m_view_archiveDirs->setCurrentIndex(m_proxy_archive_dirs->mapFromSource(m_proxy_file_list->mapToSource(fixed_index)));
            m_view_archiveDirs->expand(m_view_archiveDirs->currentIndex());
        }
    }
    else
    {
        const QModelIndex source_index = m_proxy_file_list->mapToSource(fixed_index);
        if (m_model_filesystem->isDir(source_index) || Helper::isArchiveFile(m_model_filesystem->fileInfo(source_index)))
        {
            m_view_filesystem->setCurrentIndex(m_proxy_containers->mapFromSource(source_index));
            m_view_filesystem->expand(m_view_filesystem->currentIndex());
        }
    }
}

/* Start Thumbnails */

void ViewFiles::setThumbnailsSize(const QSize &size)
{
    if (m_thumb_size != size)
    {
        m_thumb_size = size;
        m_thumbnail_delegate->setThumbnailSize(size);
        showThumbnails();
    }
}

void ViewFiles::showThumbnails()
{

#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::showThumbnails" << m_show_thumbnails << m_fileinfo_current.getDebugInfo();
#endif
    if (m_show_thumbnails)
    {
        m_thumbnail_delegate->cancelThumbnailGeneration();
        if (m_view_current->model()->rowCount(m_view_current->rootIndex()) == 0)
        {
            return;
        }

        QModelIndexList indexes;
        for (int i = 0; i < m_view_current->model()->rowCount(m_view_current->rootIndex()); ++i)
        {
            indexes.append(m_view_current->model()->index(i, 0, m_view_current->rootIndex()));
//            qDebug() << "\t" << m_view_current->model()->index(i, 0, m_view_current->rootIndex()).data().toString();
        }

        on_rows_inserted(indexes);
    }
}


void ViewFiles::on_rows_inserted(const QModelIndexList &indexes)
{

#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::on_rows_inserted" << m_show_thumbnails;
#endif
    if (!m_show_thumbnails)
    {
        return;
    }

    for (int i = 0; i < indexes.size(); ++i)
    {
        const QModelIndex fixed_index = m_proxy_file_list->mapToSource(indexes.at(i));
        m_thumbnail_delegate->updateThumbnail(FileInfo(fixed_index.data(QFileSystemModel::FilePathRole).toString()), fixed_index);
    }
}

void ViewFiles::setShowThumbnails(const bool b)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::setShowThumbnails" << b;
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

QFileSystemModel *ViewFiles::getFilesystemModel() const
{
    return m_model_filesystem;
}

void ViewFiles::on_thumbnail_finished(const QModelIndex &index)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::on_thumbnail_finished" << index.internalId() << index.data().toString();
#endif

    const QModelIndex fixed_index = m_proxy_file_list->mapFromSource(index);
    m_view_current->update(fixed_index);
    m_view_current->updateGeometry();
    m_view_current->doItemsLayout();
}

/* End Thumbnails */


QVariant ViewFiles::FileSystemModel::data(const QModelIndex &index, int role) const
{
    if (Qt::TextAlignmentRole == role)
    {
        if (index.column() == 1)
        {
            return int(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    return QFileSystemModel::data(index, role);
}

bool ViewFiles::FileListSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
//    if (qobject_cast<const ArchiveModel*>(left.model()))
//    {
//        int left_type = left.data(Helper::ROLE_TYPE).toInt();
//        int right_type = right.data(Helper::ROLE_TYPE).toInt();

//        if (left_type == right_type)  // if both indexes are same type, compare by name
//        {
//            QString left_name = left.sibling(left.row(), ArchiveItem::col_name).data(Qt::EditRole).toString();
//            QString right_name = right.sibling(right.row(), ArchiveItem::col_name).data(Qt::EditRole).toString();

//            return (left_name.compare(right_name, Qt::CaseInsensitive) < 0);
//        }

//        if (left_type == Helper::TYPE_ARCHIVE_DIR)
//        {
//            return true;
//        }
//        else if (right_type == Helper::TYPE_ARCHIVE_DIR)
//        {
//            return false;
//        }

//        return true;
//    }
//    else if (const QFileSystemModel *fsm = qobject_cast<const QFileSystemModel*>(left.model()))
//    {
//        QFileInfo left_fileinfo = fsm->fileInfo(left);
//        QFileInfo right_fileinfo = fsm->fileInfo(right);
//        if (left_fileinfo.isDir())
//        {
//            if (Helper::isImageFile(right_fileinfo) || Helper::isArchiveFile(right_fileinfo))
//            {
//                return true;
//            }
//        }
//        else if (right_fileinfo.isDir())
//        {
//            return false;
//        }

//        return (left_fileinfo.fileName().compare(right_fileinfo.fileName(), Qt::CaseInsensitive) < 0);

//    }

    return QSortFilterProxyModel::lessThan(left, right);

}

bool ViewFiles::ContainersSortFilterProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);
    return (source_column == 0);
}

bool ViewFiles::ContainersSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (const QFileSystemModel *fsm = qobject_cast<const QFileSystemModel*>(this->sourceModel()))
    {
        const QFileInfo fi = fsm->fileInfo(fsm->index(source_row, 0, source_parent));
        if (fi.isDir() || Helper::isArchiveFile(fi))
        {
            return true;
        }
        return false;
    }
    return true;
}

bool ViewFiles::ArchiveDirsSortFilterProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);
    return (source_column == 0);
}

bool ViewFiles::ArchiveDirsSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const QModelIndex index0 = this->sourceModel()->index(sourceRow, 0, sourceParent);
    return !(index0.data(Helper::ROLE_TYPE).toInt() == Helper::TYPE_ARCHIVE_FILE);
}
