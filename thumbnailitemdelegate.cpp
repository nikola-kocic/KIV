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


ThumbnailItemDelegate::ThumbnailItemDelegate(QSize thumbSize, QObject *parent)
    : QStyledItemDelegate(parent)
{
    m_thumb_size = thumbSize;
    m_watcherThumbnail = new QFutureWatcher<QImage>(this);
    connect(m_watcherThumbnail, SIGNAL(resultReadyAt(int)), this, SLOT(showThumbnail(int)));
}

QSize ThumbnailItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size_grid = QSize(m_thumb_size.width() + 6, m_thumb_size.height() + option.fontMetrics.height() + 10);
    return size_grid;
}

void ThumbnailItemDelegate::updateThumbnail(FileInfo info, QModelIndex index)
{
    QByteArray filepath = index.data(QFileSystemModel::FilePathRole).toByteArray();
    qDebug() << filepath;
    QByteArray path_hash = QCryptographicHash::hash(filepath, QCryptographicHash::Md4);
    if (m_thumbnails.contains(path_hash))
    {
        // TODO: check if modified date maches
        return;
    }
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
        qDebug() << QDateTime::currentDateTime() << "ThumbnailItemDelegate::updateThumbnail" << index.internalId() << info.getFilePath();
        qDebug() << info.getDebugInfo();
#endif
    FileInfo pli_info;

//    int type = index.data(ROLE_TYPE).toInt();
    QString name = index.data(Qt::DisplayRole).toString();
    pli_info = info;
    if (info.fileExists())
    {
        if (!info.isArchive())
        {
            pli_info.image.setFile(pli_info.image.dir(), name);
        }
        else
        {
            pli_info.zipImageFileName = name;
        }
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
        qDebug() << QDateTime::currentDateTime() << "ThumbnailItemDelegate::updateThumbnail" << "insert to files"  << index.data().toString() << info.getFilePath();
#endif
        ThumbnailInfo(pli_info, m_thumb_size);

        m_thumbnails.insert(path_hash, QIcon(QPixmap::fromImage(PictureLoader::getThumbnail(ThumbnailInfo(pli_info, m_thumb_size)))));
        emit thumbnailFinished(index);
    }
    else
    {
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
        qDebug() << QDateTime::currentDateTime() << "ThumbnailItemDelegate::updateThumbnail" << "insert to dirs"  <<  index.data().toString() << info.getFilePath();
#endif
        QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
        QImage thumb = PictureLoader::styleThumbnail(icon.pixmap(icon.availableSizes().last()).toImage(), ThumbnailInfo(info, m_thumb_size));
        icon = QIcon(QPixmap::fromImage(thumb));
        m_thumbnails.insert(path_hash, icon);
    }
}

void ThumbnailItemDelegate::setThumbnailSize(const QSize &size)
{
    m_thumb_size = size;
}

void ThumbnailItemDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

    QByteArray path_hash = QCryptographicHash::hash(index.data(QFileSystemModel::FilePathRole).toByteArray(), QCryptographicHash::Md4);
//    qDebug() << index << index.data();
    if (m_thumbnails.contains(path_hash))
    {
        if (QStyleOptionViewItemV4 *v4 = qstyleoption_cast<QStyleOptionViewItemV4 *>(option))
        {
            v4->icon = m_thumbnails.value(path_hash, QIcon());
            v4->decorationSize = v4->icon.availableSizes().last();
        }
    }
}

void ThumbnailItemDelegate::showThumbnail(int num)
{
//#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
//    qDebug() << QDateTime::currentDateTime() << "ThumbnailItemDelegate::showThumbnail" << m_indexes.at(num) << m_indexes.at(num).data();
//#endif
//    m_thumbnails.insert(m_indexes.at(num), QIcon(QPixmap::fromImage(m_watcherThumbnail->resultAt(num))));
//    emit thumbnailFinished(m_indexes.at(num));
//    m_indexes.removeAt(num);
//    ++m_thumbs_return_count;
//    if (m_thumbs_return_count == m_files.size())
//    {
//        m_indexes.clear();
//        m_files.clear();
//        m_thumbs_return_count = 0;
//        m_watcherThumbnail->setFuture(QFuture<QImage>());
//    }
}
