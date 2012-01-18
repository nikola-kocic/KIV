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
    , m_thumb_size(thumbSize)
    , m_watcherThumbnail(new QFutureWatcher<QImage>(this))
{
    connect(m_watcherThumbnail, SIGNAL(resultReadyAt(int)), this, SLOT(showThumbnail(int)));
}

void ThumbnailItemDelegate::setThumbnailSize(const QSize &size)
{
    if (size != m_thumb_size)
    {
        m_thumb_size = size;
        clearThumbnailsCache();
    }
}

QSize ThumbnailItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QSize &size_grid = QSize(m_thumb_size.width() + 6, m_thumb_size.height() + option.decorationSize.height());
    return size_grid;
}

void ThumbnailItemDelegate::cancelThumbnailGeneration()
{
    if (!m_files_to_process.isEmpty())
    {
        const ProcessInfo oldIndex = m_files_to_process.at(0);
        m_files_to_process.clear();
        m_files_to_process.append(oldIndex);
    }
}

void ThumbnailItemDelegate::updateThumbnail(const FileInfo &info, const QModelIndex &index)
{
    QDateTime currentDateTime;
    const QByteArray &filepath = index.data(QFileSystemModel::FilePathRole).toByteArray();
    const QByteArray &path_hash = QCryptographicHash::hash(filepath, QCryptographicHash::Md4);
    const QVariant &date = index.data(Helper::ROLE_FILE_DATE);

#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
        qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ThumbnailItemDelegate::updateThumbnail" << filepath;
#endif

    if (date.isValid())
    {
        currentDateTime = date.toDateTime();
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
        qDebug("Date from QVariant");
#endif
    }
    else
    {
        if (const QFileSystemModel *fsm = qobject_cast<const QFileSystemModel *>(index.model()))
        {
            currentDateTime = fsm->lastModified(index);
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
            qDebug("Date from QFileSystemModel"_;
#endif
        }
        else
        {
            QFileInfo fi(filepath);
            currentDateTime = fi.lastModified();
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
            qDebug("Date from QFileInfo");
#endif
        }
    }

#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
    qDebug() << currentDateTime;
#endif

    if (m_thumbnails.contains(path_hash))
    {
        ThumbImageDate ti = m_thumbnails.value(path_hash);
        if (ti.date == currentDateTime)
        {
            return;
        }
    }

#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
        qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ThumbnailItemDelegate::updateThumbnail" << "generating for" << filepath;
#endif

    QIcon icon;

    if (info.fileExists())
    {
        m_files_to_process.append(ProcessInfo(index, info, path_hash, currentDateTime));
        if (m_files_to_process.size() == 1)
        {
            m_watcherThumbnail->setFuture(QtConcurrent::run(PictureLoader::getThumbnail, ThumbnailInfo(info, m_thumb_size)));
        }
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

void ThumbnailItemDelegate::clearThumbnailsCache()
{
    m_files_to_process.clear();
    m_thumbnails.clear();
}

void ThumbnailItemDelegate::showThumbnail(int num)
{
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ThumbnailItemDelegate::showThumbnail" << m_currentIndex.data().toString();
#endif

    if (m_files_to_process.isEmpty())
    {
        m_watcherThumbnail->setFuture(QFuture<QImage>());
        return;
    }

    ThumbImageDate tid;
    tid.thumb = QIcon(QPixmap::fromImage(m_watcherThumbnail->resultAt(num)));
    tid.date = m_files_to_process.at(0).getDate();
    m_thumbnails.insert(m_files_to_process.at(0).getPathHash(), tid);

    emit thumbnailFinished(m_files_to_process.at(0).getIndex());

    m_files_to_process.removeFirst();

    if (m_files_to_process.size() > 0)
    {
        m_watcherThumbnail->setFuture(QtConcurrent::run(PictureLoader::getThumbnail, ThumbnailInfo(m_files_to_process.at(0).getFileInfo(), m_thumb_size)));
    }
    else
    {
        m_files_to_process.clear();
        m_watcherThumbnail->setFuture(QFuture<QImage>());
    }

}
