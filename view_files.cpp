#include "view_files.h"

#include <QLayout>

//#define DEBUG_VIEW_FILES

#ifdef DEBUG_VIEW_FILES
#include <QDebug>
#include <QTime>
#endif

ViewFiles::ViewFiles(QWidget *parent)
    : QWidget(parent)
{

//    m_fileinfo_current = info;
    m_model_archive_files = new FilesModel(this);
    m_show_thumbnails = false;

    m_thumb_size = QSize(100, 100);

    m_listView_files = 0;
    m_treeView_files = 0;



    /* Start modelFilesystem */
    QStringList filters;
    QStringList filtersArchive = FileInfo::getFiltersArchive();
    for (int i = 0; i < filtersArchive.size(); ++i)
    {
        filters.append("*." + filtersArchive.at(i));
    }
    QStringList filtersImage = FileInfo::getFiltersImage();
    for (int i = 0; i < filtersImage.size(); ++i)
    {
        filters.append("*." + filtersImage.at(i));
    }

    m_model_filesystem = new QFileSystemModel(this);
    m_model_filesystem->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    m_model_filesystem->setNameFilterDisables(false);
    m_model_filesystem->setNameFilters(filters);
    m_model_filesystem->setRootPath("");
    /* End modelFilesystem */

//    m_model_current = m_model_files;
    m_model_current = m_model_filesystem;
    m_in_archive = false;



    m_splitter = new QSplitter(Qt::Vertical, this);

    //add item delegate
    m_thumbnail_delegate = new ThumbnailItemDelegate(m_thumb_size, this);
    connect(m_thumbnail_delegate, SIGNAL(thumbnailFinished(QModelIndex)), this, SLOT(on_thumbnail_finished(QModelIndex)));

    /* Start archiveDirsView */
    m_view_archiveDirs = new ViewArchiveDirs();
    m_view_archiveDirs->hide();

    m_splitter->addWidget(m_view_archiveDirs);
    m_splitter->setSizes(QList<int>() << 100);

    m_view_archiveDirs->setModel(m_model_archive_files);

    connect(m_view_archiveDirs, SIGNAL(currentRowChanged(QModelIndex)), this, SLOT(on_archiveDirsView_currentRowChanged(QModelIndex)));

    /* End archiveDirsView */

    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setSpacing(0);
    layoutMain->setMargin(0);
    layoutMain->addWidget(m_splitter);
    this->setLayout(layoutMain);

    m_view_mode = FileViewMode::List;
    initViewItem();
}

FileInfo ViewFiles::getCurrentFileInfo() const
{
    return m_fileinfo_current;
}


void ViewFiles::initViewItem()
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::initViewItem" << m_view_mode;
#endif
    if (m_view_mode == FileViewMode::Details)
    {
        m_treeView_files = new TreeViewFiles(this);
        m_listView_files = 0;
        m_view_current = qobject_cast<QAbstractItemView *>(m_treeView_files);
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
    m_view_current->setModel(m_model_current);
    connect(m_view_current->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex,QModelIndex)));
    if (m_fileinfo_current.isZip())
    {
        m_view_current->setRootIndex(m_view_archiveDirs->currentIndex());
    }
    connect(m_view_current, SIGNAL(activated(QModelIndex)), this, SLOT(on_item_activated(QModelIndex)));
    m_splitter->addWidget(m_view_current);

    QSizePolicy policyV(QSizePolicy::Preferred, QSizePolicy::Expanding);
    policyV.setHorizontalStretch(0);
    policyV.setVerticalStretch(1);
    m_view_current->setSizePolicy(policyV);

    setShowThumbnails(m_show_thumbnails);
}

void ViewFiles::setViewMode(int mode)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::setViewMode" << mode;
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
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::setCurrentFile" << "start" << "old m_in_archive" << m_in_archive;
#endif
    m_fileinfo_current = info;

    showThumbnails();

    if (info.isZip())
    {
        m_model_archive_files->setPath(info);
        if (m_in_archive == false)
        {
            m_in_archive = true;
            m_view_current->selectionModel()->disconnect();
            m_model_current = m_model_archive_files;
            m_view_current->setModel(m_model_current);
            connect(m_view_current->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex,QModelIndex)));
            m_view_archiveDirs->show();
        }
        QModelIndex dirIndex = m_model_archive_files->getDirectory(info.zipPath);
        m_view_archiveDirs->setCurrentIndexFromSource(dirIndex);

        if (info.fileExists())
        {
            m_view_current->setCurrentIndex(m_model_archive_files->findIndexChild(info.zipImageFileName, dirIndex));
        }
    }
    else
    {
        if (m_in_archive == true)
        {
            m_in_archive = false;
            m_view_current->selectionModel()->disconnect();
            m_model_current = m_model_filesystem;
            m_view_current->setModel(m_model_current);
            connect(m_view_current->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex,QModelIndex)));
            m_view_archiveDirs->hide();
        }
        m_view_current->setRootIndex(m_model_filesystem->index(info.containerPath));
        m_view_current->setCurrentIndex(m_model_filesystem->index(info.getFilePath()));
    }

    m_view_current->scrollTo(m_view_current->currentIndex());

#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::setCurrentFile end" << info.getFilePath() << "archive?" << m_in_archive;
    qDebug() << m_fileinfo_current.getDebugInfo();
#endif
    emit currentFileChanged(m_fileinfo_current);
}


void ViewFiles::on_archiveDirsView_currentRowChanged(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }

    QString pathToImage = "";
    QModelIndex cindex = index;
    while (cindex.parent().isValid())
    {
        pathToImage = cindex.data(Qt::DisplayRole).toString() + "/" + pathToImage;
        cindex = cindex.parent();
    }
    if (pathToImage.isEmpty()) pathToImage = "/";
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::on_archiveDirsView_currentRowChanged" << index.data().toString() << "pathToImage" << pathToImage;
#endif
    m_fileinfo_current.zipPath = pathToImage;
    m_fileinfo_current.zipImageFileName.clear();
    m_view_current->setRootIndex(index);
    m_view_current->selectionModel()->clear();
//    showThumbnails();
#ifdef DEBUG_VIEW_FILES
    qDebug() << m_fileinfo_current.getDebugInfo();
#endif
    emit currentFileChanged(m_fileinfo_current);
}

void ViewFiles::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QModelIndex index;
    if (current.column() > 0)
    {
        index = current.sibling(current.row(), 0);
    }
    else
    {
        index = current;
    }
    m_view_current->scrollTo(index);
    QString filename = index.data(Qt::DisplayRole).toString();

    if (m_in_archive)
    {
        int type = index.data(ROLE_TYPE).toInt();
        if (type == TYPE_ARCHIVE_FILE)
        {
            m_fileinfo_current.zipImageFileName = filename;
        }
        else
        {
            m_fileinfo_current.zipImageFileName.clear();
            m_fileinfo_current.imageFileName.clear();
        }
    }
    else
    {
        if (m_model_filesystem->isDir(current))
        {
            m_fileinfo_current.zipImageFileName.clear();
            m_fileinfo_current.imageFileName.clear();
        }
        else
        {
            m_fileinfo_current.imageFileName = filename;
            m_fileinfo_current.zipImageFileName.clear();
            m_fileinfo_current.zipPath.clear();
        }
    }

#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::currentChanged" << index.internalId() << filename << "archive?" << m_in_archive;
    qDebug() << m_fileinfo_current.getDebugInfo();
#endif

    emit currentFileChanged(m_fileinfo_current);
}

void ViewFiles::pageNext()
{
    if (!m_view_current->currentIndex().isValid()) return;

    if (m_in_archive)
    {
        for (int i = m_view_current->currentIndex().row() + 1; i < m_view_current->model()->rowCount(m_view_current->rootIndex()); ++i)
        {
            int type = m_view_current->model()->index(i, 0, m_view_current->rootIndex()).data(ROLE_TYPE).toInt();
            if (type == TYPE_ARCHIVE_FILE)
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
            if (isImage(m_model_filesystem->fileInfo(index)))
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

    if (m_in_archive)
    {
        for (int i = m_view_current->currentIndex().row() - 1; i >= 0; --i)
        {
            int type = m_view_current->model()->index(i, 0, m_view_current->rootIndex()).data(ROLE_TYPE).toInt();
            if (type == TYPE_ARCHIVE_FILE)
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
            if (isImage(m_model_filesystem->fileInfo(index)))
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
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::on_item_activated" <<index.internalId() <<  index.data().toString();
#endif
    if (m_in_archive)
    {
        if (index.data(ROLE_TYPE).toInt() == TYPE_ARCHIVE_DIR)
        {
            m_view_archiveDirs->setCurrentIndexFromSource(index);
        }
    }
    else
    {
        if (m_model_filesystem->isDir(index) || isArchive(m_model_filesystem->fileInfo(index)))
        {

            emit activated(m_model_filesystem->filePath(index));
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
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::showThumbnails" << m_show_thumbnails;
#endif
    if (m_show_thumbnails)
    {
        if (m_view_current->model()->rowCount(m_view_current->rootIndex()) == 0)
        {
            return;
        }

        QModelIndexList indexes;
        for (int i = 0; i < m_view_current->model()->rowCount(m_view_current->rootIndex()); ++i)
        {
            indexes.append(m_view_current->model()->index(i, 0, m_view_current->rootIndex()));
        }

        on_rows_inserted(indexes);
    }
}


void ViewFiles::on_rows_inserted(const QModelIndexList &indexes)
{

#ifdef DEBUG_VIEW_FILES
//    qDebug() << QDateTime::currentDateTime() << "ViewFiles::on_rows_inserted" << m_show_thumbnails;
#endif
    if (!m_show_thumbnails)
    {
        return;
    }

    for (int i = 0; i < indexes.size(); ++i)
    {
        FileInfo pli_info = m_fileinfo_current;
        QString name = indexes.at(i).data(Qt::DisplayRole).toString();
        QFileInfo fi = m_model_filesystem->fileInfo(indexes.at(i));
        if (isImage(fi))
        {
            pli_info.imageFileName = name;
        }
        else
        {
            pli_info.containerPath += "/" + name;
        }

        m_thumbnail_delegate->updateThumbnail(ThumbnailInfo(pli_info, m_thumb_size), indexes.at(i));
    }
}

void ViewFiles::setShowThumbnails(bool b)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::setShowThumbnails" << b;
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
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::on_thumbnail_finished" << index.internalId() << index.data().toString();
#endif

    m_view_current->update(index);
}

/* End Thumbnails */
