#include "view_files.h"
#include "archive_item.h"

#include <QLayout>

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
    , m_thumb_size(QSize(100, 100))

    , m_view_archiveDirs(new ViewArchiveDirs())
    , m_treeView_files(0)
    , m_listView_files(0)
    , m_view_current(0)

    , m_model_archive_files(new ArchiveModel(this))
    , m_model_filesystem(new FileSystemModel(this))
    , m_proxy_sort(new MySortFilterProxyModel(this))

    , m_splitter(new QSplitter(Qt::Vertical, this))
    , m_thumbnail_delegate(new ThumbnailItemDelegate(m_thumb_size, this))

{
    /* Start modelFilesystem */
    QStringList filters;
    QStringList filtersArchive = Helper::getFiltersArchive();
    for (int i = 0; i < filtersArchive.size(); ++i)
    {
        filters.append("*." + filtersArchive.at(i));
    }
    QStringList filtersImage = Helper::getFiltersImage();
    for (int i = 0; i < filtersImage.size(); ++i)
    {
        filters.append("*." + filtersImage.at(i));
    }

    m_model_filesystem->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    m_model_filesystem->setNameFilterDisables(false);
    m_model_filesystem->setNameFilters(filters);
    m_model_filesystem->setRootPath("");
    /* End modelFilesystem */

    connect(m_thumbnail_delegate, SIGNAL(thumbnailFinished(QModelIndex)), this, SLOT(on_thumbnail_finished(QModelIndex)));

    /* Start archiveDirsView */
    m_view_archiveDirs->hide();

    m_splitter->addWidget(m_view_archiveDirs);
    m_splitter->setSizes(QList<int>() << 100);

    m_proxy_sort->setSortRole(Qt::EditRole);
    m_view_archiveDirs->setModel(m_proxy_sort);


    connect(m_view_archiveDirs, SIGNAL(currentRowChanged(QModelIndex)), this, SLOT(on_archiveDirsView_currentRowChanged(QModelIndex)));

    /* End archiveDirsView */

    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setSpacing(0);
    layoutMain->setMargin(0);
    layoutMain->addWidget(m_splitter);
    this->setLayout(layoutMain);

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
    m_view_current->setModel(m_proxy_sort);
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

    if (info.isInArchive())
    {
        m_model_archive_files->setPath(info.getContainerPath());
        if (!m_fileinfo_current.isInArchive() || !m_fileinfo_current.isValid())
        {
            m_proxy_sort->setSourceModel(0); // crashes without this
            m_proxy_sort->setSourceModel(m_model_archive_files);
            m_view_archiveDirs->setModel(m_proxy_sort);
            m_view_archiveDirs->show();
        }
        QModelIndex dirIndex = m_model_archive_files->getDirectory(info.getZipPath());
        m_view_archiveDirs->setCurrentIndexFromSource(m_proxy_sort->mapFromSource(dirIndex));

        if (info.fileExists())
        {
            m_view_current->setCurrentIndex(m_proxy_sort->mapFromSource(m_model_archive_files->findIndexChild(info.getImageFileName(), dirIndex)));
        }
    }
    else
    {
        if (m_fileinfo_current.isInArchive() || !m_fileinfo_current.isValid())
        {
            m_model_archive_files->clear();
            m_proxy_sort->setSourceModel(m_model_filesystem);
            m_view_archiveDirs->setModel(0);
            m_view_archiveDirs->hide();
        }
        m_view_current->setRootIndex(m_proxy_sort->mapFromSource(m_model_filesystem->index(info.getContainerPath())));
        m_view_current->setCurrentIndex(m_proxy_sort->mapFromSource(m_model_filesystem->index(info.getPath())));
    }

    m_proxy_sort->sort(0);
    m_view_current->scrollTo(m_view_current->currentIndex());

#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::setCurrentFile end" << info.getDebugInfo();
#endif
    m_fileinfo_current = info;
    emit currentFileChanged(m_fileinfo_current);

    showThumbnails();
}


void ViewFiles::on_archiveDirsView_currentRowChanged(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }
    m_fileinfo_current = FileInfo(index.data(QFileSystemModel::FilePathRole).toString());

    m_view_current->setRootIndex(index);
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
            int type = m_view_current->model()->index(i, 0, m_view_current->rootIndex()).data(Helper::ROLE_TYPE).toInt();
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
            const QModelIndex &index = m_view_current->model()->index(i, 0, m_view_current->rootIndex());
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
            int type = m_view_current->model()->index(i, 0, m_view_current->rootIndex()).data(Helper::ROLE_TYPE).toInt();
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
            const QModelIndex &index = m_view_current->model()->index(i, 0, m_view_current->rootIndex());
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
    QModelIndex fixed_index;
    if (index.column() > 0)
    {
        fixed_index = index.sibling(index.row(), 0);
    }
    else
    {
        fixed_index = index;
    }

    if (m_fileinfo_current.isInArchive())
    {
        if (fixed_index.data(Helper::ROLE_TYPE).toInt() == Helper::TYPE_ARCHIVE_DIR)
        {
            m_view_archiveDirs->setCurrentIndexFromSource(fixed_index);
        }
    }
    else
    {
        fixed_index = m_proxy_sort->mapToSource(fixed_index);
        if (m_model_filesystem->isDir(fixed_index) || Helper::isArchiveFile(m_model_filesystem->fileInfo(fixed_index)))
        {
            emit activated(m_model_filesystem->filePath(fixed_index));
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
        QModelIndex fixed_index = m_proxy_sort->mapToSource(indexes.at(i));
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

void ViewFiles::on_thumbnail_finished(const QModelIndex &index)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ViewFiles::on_thumbnail_finished" << index.internalId() << index.data().toString();
#endif

    QModelIndex fixed_index = m_proxy_sort->mapFromSource(index);
    m_view_current->update(fixed_index);
    m_view_current->updateGeometry();
    m_view_current->doItemsLayout();
}

FileSystemModel::FileSystemModel(QObject *parent) : QFileSystemModel(parent)
{
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
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

ViewFiles::MySortFilterProxyModel::MySortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

bool ViewFiles::MySortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (qobject_cast<const ArchiveModel*>(left.model()))
    {
        int left_type = left.data(Helper::ROLE_TYPE).toInt();
        int right_type = right.data(Helper::ROLE_TYPE).toInt();

        if (left_type == right_type)  // if both indexes are same type, compare by name
        {
            QString left_name = left.sibling(left.row(), ArchiveItem::col_name).data(Qt::EditRole).toString();
            QString right_name = right.sibling(right.row(), ArchiveItem::col_name).data(Qt::EditRole).toString();

            return (left_name.compare(right_name, Qt::CaseInsensitive) < 0);
        }

        if (left_type == Helper::TYPE_ARCHIVE_DIR)
        {
            return true;
        }
        else if (right_type == Helper::TYPE_ARCHIVE_DIR)
        {
            return false;
        }

        return true;
    }
    else if (const FileSystemModel *fsm = qobject_cast<const FileSystemModel*>(left.model()))
    {
        QFileInfo left_fileinfo = fsm->fileInfo(left);
        QFileInfo right_fileinfo = fsm->fileInfo(right);
        if (left_fileinfo.isDir())
        {
            if (Helper::isImageFile(right_fileinfo) || Helper::isArchiveFile(right_fileinfo))
            {
                return true;
            }
        }
        else if (right_fileinfo.isDir())
        {
            return false;
        }

        return (left_fileinfo.fileName().compare(right_fileinfo.fileName(), Qt::CaseInsensitive) < 0);

    }

    return QSortFilterProxyModel::lessThan(left, right);

}

/* End Thumbnails */
