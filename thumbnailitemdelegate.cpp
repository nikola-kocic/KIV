#include "thumbnailitemdelegate.h"

#include <QtConcurrentRun>
#include <QFileIconProvider>
#include <QPixmap>
#include <QAbstractItemView>
#include <QtConcurrentMap>
#include <QCryptographicHash>
#include <QDir>
#include <QFileSystemModel>

//#define DEBUG_THUMBNAIL_ITEM_DELEGATE

#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
#include <QDebug>
#include <QTime>
#endif


ThumbnailItemDelegate::ThumbnailItemDelegate(const QSize &thumbSize, QObject *parent)
    : QStyledItemDelegate(parent)
{
//    m_flag_cancel = false;
    m_thumb_size = thumbSize;
    m_watcherThumbnail = new QFutureWatcher<QImage>(this);
    connect(m_watcherThumbnail, SIGNAL(resultReadyAt(int)), this, SLOT(showThumbnail(int)));
}

void ThumbnailItemDelegate::setThumbnailSize(const QSize &size)
{
    m_thumb_size = size;
}

QSize ThumbnailItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QSize &size_grid = QSize(m_thumb_size.width() + 6, m_thumb_size.height() + option.fontMetrics.height() + 10);
    return size_grid;
}

void ThumbnailItemDelegate::cancelThumbnailGeneration()
{
    if (m_watcherThumbnail->isRunning())
    {
        m_canceledFiles.append(m_files_to_process.at(getArrayIndexOfModelIndex(m_currentIndex)));

        m_currentIndex = QModelIndex();
        m_files_to_process.clear();
        m_watcherThumbnail->setFuture(QFuture<QImage>());
    }
}

void ThumbnailItemDelegate::updateThumbnail(const FileInfo &info, const QModelIndex &index)
{
    QDateTime currentDateTime;
    const QByteArray &filepath = index.data(QFileSystemModel::FilePathRole).toByteArray();
    const QByteArray &path_hash = QCryptographicHash::hash(filepath, QCryptographicHash::Md4);
    const QVariant &date = index.data(ROLE_FILE_DATE);

#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
        qDebug() << QDateTime::currentDateTime() << "ThumbnailItemDelegate::updateThumbnail" << filepath;
#endif

    if (date.isValid())
    {
        currentDateTime = date.toDateTime();
    }
    else
    {
        QFileInfo fi(filepath);
        currentDateTime = fi.lastModified();
    }

    if (m_thumbnails.contains(path_hash))
    {
        ThumbImageDate ti = m_thumbnails.value(path_hash);
        if (ti.date == currentDateTime)
        {
            return;
        }
    }

#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
        qDebug() << QDateTime::currentDateTime() << "ThumbnailItemDelegate::updateThumbnail" << "generating for" << filepath;
#endif

    QIcon icon;

    if (info.fileExists())
    {
        ProcessInfo pi(index, info, path_hash, currentDateTime);
        m_files_to_process.append(pi);
        if (!m_currentIndex.isValid())
        {
            m_currentIndex = index;
            m_watcherThumbnail->setFuture(QtConcurrent::run(PictureLoader::getThumbnail, ThumbnailInfo(info, m_thumb_size)));
        }
//        icon = QIcon(QPixmap::fromImage(PictureLoader::getThumbnail(ThumbnailInfo(info, m_thumb_size))));
    }
    else
    {
        icon = index.data(Qt::DecorationRole).value<QIcon>();
        QImage thumb = PictureLoader::styleThumbnail(icon.pixmap(icon.availableSizes().last()).toImage(), ThumbnailInfo(info, m_thumb_size));
        icon = QIcon(QPixmap::fromImage(thumb));

        ThumbImageDate tid;
        tid.thumb = icon;
        tid.date = currentDateTime;
        m_thumbnails.insert(path_hash, tid);
        emit thumbnailFinished(index);
    }
}


void ThumbnailItemDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

    const QByteArray &path_hash = QCryptographicHash::hash(index.data(QFileSystemModel::FilePathRole).toByteArray(), QCryptographicHash::Md4);
//    qDebug() << index << index.data();
    if (m_thumbnails.contains(path_hash))
    {
        if (QStyleOptionViewItemV4 *v4 = qstyleoption_cast<QStyleOptionViewItemV4 *>(option))
        {
            ThumbImageDate tid = m_thumbnails.value(path_hash);
            if (tid.thumb.availableSizes().isEmpty())
            {
                return;
            }
            v4->icon = tid.thumb;
            v4->decorationSize = v4->icon.availableSizes().last();
        }
    }
}

void ThumbnailItemDelegate::showThumbnail(int num)
{
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
    qDebug() << QDateTime::currentDateTime() << "ThumbnailItemDelegate::showThumbnail" << m_currentIndex.data().toString();
#endif

    if (!m_currentIndex.isValid() && !m_canceledFiles.isEmpty())
    {
        ThumbImageDate tid;
        tid.thumb = QIcon(QPixmap::fromImage(m_watcherThumbnail->resultAt(num)));
        tid.date = m_canceledFiles.at(0).getDate();
        m_thumbnails.insert(m_canceledFiles.at(0).getPathHash(), tid);

        emit thumbnailFinished(m_canceledFiles.at(0).getIndex());
        m_canceledFiles.clear();
        return;
    }

    int currentIndex = getArrayIndexOfModelIndex(m_currentIndex);

    ThumbImageDate tid;
    tid.thumb = QIcon(QPixmap::fromImage(m_watcherThumbnail->resultAt(num)));
    tid.date = m_files_to_process.at(currentIndex).getDate();
    m_thumbnails.insert(m_files_to_process.at(currentIndex).getPathHash(), tid);

    emit thumbnailFinished(m_currentIndex);

    m_files_to_process.removeAt(currentIndex);

    if (m_files_to_process.size() > 0)
    {
        m_currentIndex = m_files_to_process.at(0).getIndex();
        m_watcherThumbnail->setFuture(QtConcurrent::run(PictureLoader::getThumbnail, ThumbnailInfo(m_files_to_process.at(0).getFileInfo(), m_thumb_size)));
    }
    else
    {
        m_currentIndex = QModelIndex();
        m_files_to_process.clear();
        m_watcherThumbnail->setFuture(QFuture<QImage>());
    }

}

int ThumbnailItemDelegate::getArrayIndexOfModelIndex(const QModelIndex &index)
{
    for (int i = 0; i < m_files_to_process.size(); ++i)
    {
        if (m_files_to_process.at(i).getIndex() == index)
        {
            return i;
        }
    }
    return 0;
}
