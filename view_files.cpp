#include "view_files.h"
#include "settings.h"

#include <QFileIconProvider>
#include <QLayout>

//#define DEBUG_VIEW_FILES

#ifdef DEBUG_VIEW_FILES
#include <QDebug>
#include <QTime>
#endif

ViewFiles::ViewFiles(QAbstractItemModel *model, QWidget *parent)
{
    m_model = model;
    m_thumb_size = QSize(200, 200);

    m_listView_files = 0;
    m_treeView_files = 0;

    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setSpacing(0);
    layoutMain->setMargin(0);
    this->setLayout(layoutMain);

    m_mode = QListView::ListMode;
    initViewItem();
}

void ViewFiles::setCurrentIndex(const QModelIndex &index)
{
#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::setCurrentIndexFromSource" << index.data();
#endif
    m_aiv->setCurrentIndex(index);
    m_aiv->scrollTo(m_aiv->currentIndex());
}

FileInfo ViewFiles::getCurrentFileInfo() const
{
    return m_currentInfo;
}


void ViewFiles::initViewItem()
{
    if (m_mode == QListView::IconMode)
    {
        m_listView_files = new ListViewFiles(this);
        m_treeView_files = 0;
        m_aiv = qobject_cast<QAbstractItemView *>(m_listView_files);
    }
    else
    {
        m_treeView_files = new TreeViewFiles(this);
        m_listView_files = 0;
        m_aiv = qobject_cast<QAbstractItemView *>(m_treeView_files);
    }

    m_aiv->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_aiv->setModel(m_model);
    if (m_currentInfo.isZip())
    {
        m_aiv->setRootIndex(m_index_current_archive_dirs);
    }
    connect(m_aiv, SIGNAL(activated(QModelIndex)), this, SIGNAL(activated(QModelIndex)));
    connect(m_aiv->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex,QModelIndex)));
    this->layout()->addWidget(m_aiv);
    if (m_mode == QListView::IconMode)
    {
        m_listView_files->startShowingThumbnails();
    }
}

void ViewFiles::setViewMode(QListView::ViewMode mode)
{
    if (m_mode != mode)
    {
        m_aiv->setModel(0);
        m_aiv->disconnect();
        m_aiv->deleteLater();

        m_mode = mode;
        initViewItem();
    }
}

void ViewFiles::setCurrentDirectory(const FileInfo &info)
{
    m_currentInfo = info;

    if (m_mode == QListView::IconMode)
    {
        m_listView_files->startShowingThumbnails();
    }
//    else
//    {
//        m_treeView_files->startShowingThumbnails();
//    }
}

void ViewFiles::setThumbnailsSize(const QSize &size)
{
    if (m_thumb_size != size)
    {
        m_thumb_size = size;
        if (m_mode == QListView::IconMode)
        {
            m_listView_files->startShowingThumbnails();
        }
//        else
//        {
//            m_treeView_files->startShowingThumbnails();
//        }
    }
}


/* This is index from ArchiveModel */
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

    m_currentInfo.zipPath = pathToImage;
    m_currentInfo.zipImageFileName.clear();
    m_aiv->setRootIndex(index);
    m_index_current_archive_dirs = index;
    m_aiv->selectionModel()->clear();
    if (m_mode == QListView::IconMode)
    {
        m_listView_files->startShowingThumbnails();
    }
//    else
//    {
//        m_treeView_files->startShowingThumbnails();
//    }
    emit currentFileChanged(m_currentInfo);
}

void ViewFiles::on_thumbnail_finished(const QModelIndex &index)
{
    if (m_mode == QListView::IconMode)
    {
        m_listView_files->update(index);
    }
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
    m_aiv->scrollTo(index);
    int type = index.data(ROLE_TYPE).toInt();
    QString filename = index.data(Qt::DisplayRole).toString();

#ifdef DEBUG_VIEW_FILES
    qDebug() << QDateTime::currentDateTime() << "ViewFiles::currentChanged" << index << filename;
#endif

    if (type == TYPE_FILE)
    {
        m_currentInfo.imageFileName = filename;
        m_currentInfo.zipImageFileName.clear();
        m_currentInfo.zipPath.clear();
    }
    else if (type == TYPE_ARCHIVE_FILE)
    {
        m_currentInfo.zipImageFileName = filename;
    }
    else
    {
        m_currentInfo.zipImageFileName.clear();
        m_currentInfo.imageFileName.clear();
    }

    emit currentFileChanged(m_currentInfo);
}

void ViewFiles::pageNext()
{
    if (!m_aiv->currentIndex().isValid()) return;

    for (int i = m_aiv->currentIndex().row() + 1; i < m_aiv->model()->rowCount(m_aiv->rootIndex()); ++i)
    {
        int type = m_aiv->model()->index(i, 0, m_aiv->rootIndex()).data(ROLE_TYPE).toInt();
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            m_aiv->setCurrentIndex(m_aiv->model()->index(i, 0, m_aiv->rootIndex()));
            break;
        }
    }
}

void ViewFiles::pagePrevious()
{
    if (!m_aiv->currentIndex().isValid()) return;

    for (int i = m_aiv->currentIndex().row() - 1; i >= 0; --i)
    {
        int type = m_aiv->model()->index(i, 0, m_aiv->rootIndex()).data(ROLE_TYPE).toInt();
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            m_aiv->setCurrentIndex(m_aiv->model()->index(i, 0, m_aiv->rootIndex()));
            break;
        }
    }
}


void ViewFiles::on_thumbnail_activated(const QModelIndex &index)
{
    emit activated(m_listView_files->getIndexFromProxy(index));
}

