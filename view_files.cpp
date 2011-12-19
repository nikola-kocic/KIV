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

    m_model_files = new FilesModel(this);
    m_show_thumbnails = false;
    m_model_current = m_model_files;
    m_thumb_size = QSize(200, 200);

    m_listView_files = 0;
    m_treeView_files = 0;

    m_splitter = new QSplitter(Qt::Vertical, this);

    //add item delegate
    m_thumbnail_delegate = new ThumbnailItemDelegate(m_thumb_size, this);
    connect(m_thumbnail_delegate, SIGNAL(thumbnailFinished(QModelIndex)), this, SLOT(on_thumbnail_finished(QModelIndex)));

    /* Start archiveDirsView */
    m_view_archiveDirs = new ViewArchiveDirs();
    m_view_archiveDirs->hide();

    m_splitter->addWidget(m_view_archiveDirs);
    m_splitter->setSizes(QList<int>() << 100);

    m_view_archiveDirs->setModel(m_model_files);

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
    }

    m_view_current->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view_current->setModel(m_model_current);
    if (m_fileinfo_current.isZip())
    {
        m_view_current->setRootIndex(m_view_archiveDirs->currentIndex());
    }
    connect(m_view_current, SIGNAL(activated(QModelIndex)), this, SLOT(on_item_activated(QModelIndex)));
    connect(m_view_current->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex,QModelIndex)));
    m_splitter->addWidget(m_view_current);

    QSizePolicy policyV(QSizePolicy::Preferred, QSizePolicy::Expanding);
    policyV.setHorizontalStretch(0);
    policyV.setVerticalStretch(1);
    m_view_current->setSizePolicy(policyV);

    setShowThumbnails(m_show_thumbnails);
}

void ViewFiles::setViewMode(int mode)
{
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
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::setCurrentFile" << info.getFilePath();
#endif
    m_fileinfo_current = info;

    m_model_files->setPath(info);
    showThumbnails();

    if (info.isZip())
    {
        QModelIndex dirIndex = m_model_files->getDirectory(info.zipPath);
        m_view_archiveDirs->setCurrentIndexFromSource(dirIndex);
        m_view_archiveDirs->show();

        if (!info.fileExists())
        {
            return;
        }

        m_view_current->setCurrentIndex(m_model_files->findIndexChild(info.zipImageFileName, dirIndex));
    }
    else
    {
        m_view_archiveDirs->hide();
        if (!info.fileExists())
        {
            return;
        }

        m_view_current->setCurrentIndex(m_model_files->findRootIndexChild(info.imageFileName));
    }

    m_view_current->scrollTo(m_view_current->currentIndex());
}


void ViewFiles::on_archiveDirsView_currentRowChanged(const QModelIndex &index)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::on_archiveDirsView_currentRowChanged" << index << index.data();
#endif
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

    m_fileinfo_current.zipPath = pathToImage;
    m_fileinfo_current.zipImageFileName.clear();
    m_view_current->setRootIndex(index);
    m_view_current->selectionModel()->clear();
    showThumbnails();
    emit currentFileChanged(m_fileinfo_current);
}

void ViewFiles::on_thumbnail_finished(const QModelIndex &index)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::on_thumbnail_finished" << index << index.data();
#endif

    m_view_current->update(index);
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
    int type = index.data(ROLE_TYPE).toInt();
    QString filename = index.data(Qt::DisplayRole).toString();

#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::currentChanged" << index << filename;
#endif

    if (type == TYPE_FILE)
    {
        m_fileinfo_current.imageFileName = filename;
        m_fileinfo_current.zipImageFileName.clear();
        m_fileinfo_current.zipPath.clear();
    }
    else if (type == TYPE_ARCHIVE_FILE)
    {
        m_fileinfo_current.zipImageFileName = filename;
    }
    else
    {
        m_fileinfo_current.zipImageFileName.clear();
        m_fileinfo_current.imageFileName.clear();
    }

    emit currentFileChanged(m_fileinfo_current);
}

void ViewFiles::pageNext()
{
    if (!m_view_current->currentIndex().isValid()) return;

    for (int i = m_view_current->currentIndex().row() + 1; i < m_view_current->model()->rowCount(m_view_current->rootIndex()); ++i)
    {
        int type = m_view_current->model()->index(i, 0, m_view_current->rootIndex()).data(ROLE_TYPE).toInt();
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            m_view_current->setCurrentIndex(m_view_current->model()->index(i, 0, m_view_current->rootIndex()));
            break;
        }
    }
}

void ViewFiles::pagePrevious()
{
    if (!m_view_current->currentIndex().isValid()) return;

    for (int i = m_view_current->currentIndex().row() - 1; i >= 0; --i)
    {
        int type = m_view_current->model()->index(i, 0, m_view_current->rootIndex()).data(ROLE_TYPE).toInt();
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            m_view_current->setCurrentIndex(m_view_current->model()->index(i, 0, m_view_current->rootIndex()));
            break;
        }
    }
}


void ViewFiles::on_item_activated(const QModelIndex &index)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::on_thumbnail_activated" <<index <<  index.data();
#endif
    if (index.data(ROLE_TYPE).toInt() == TYPE_ARCHIVE_DIR)
    {
        m_view_archiveDirs->setCurrentIndexFromSource(index);
    }
    else
    {
        emit activated(index);
    }
}

/* Start Thumbnails */

void ViewFiles::setThumbnailsSize(const QSize &size)
{
    if (m_thumb_size != size)
    {
        m_thumb_size = size;
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

        m_thumbnail_delegate->updateThumbnails(ThumbnailInfo(m_fileinfo_current, m_thumb_size), indexes);

    }
}

void ViewFiles::setShowThumbnails(bool b)
{
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

/* End Thumbnails */
