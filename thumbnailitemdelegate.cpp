#include "thumbnailitemdelegate.h"

#include <QtConcurrentRun>
#include <QFileIconProvider>
#include <QPixmap>
#include <QAbstractItemView>
#include <QtConcurrentMap>

#define DEBUG_THUMBNAIL_ITEM_DELEGATE

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

void ThumbnailItemDelegate::updateThumbnail(ThumbnailInfo thumb_info, QModelIndex index)
{
    if (m_thumbnails.contains(index.internalId()))
    {
        return;
    }

    FileInfo pli_info;

//    int type = index.data(ROLE_TYPE).toInt();
    QString name = index.data(Qt::DisplayRole).toString();
    pli_info = thumb_info.info;
//    QFileInfo fi(thumb_info.info.getFilePath());
    if (thumb_info.info.fileExists())
    {
        if (!thumb_info.info.isZip())
        {
            pli_info.imageFileName = name;
        }
        else
        {
            pli_info.zipImageFileName = name;
        }
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
        qDebug() << QDateTime::currentDateTime() << "ThumbnailItemDelegate::updateThumbnail" << "insert to files"  << index.internalId() << thumb_info.info.getFilePath();
#endif
        ThumbnailInfo ti(pli_info, thumb_info.thumbSize);

        m_thumbnails.insert(index.internalId(), QIcon(QPixmap::fromImage(PictureLoader::getThumbnail(ti))));
        emit thumbnailFinished(index);


    }
    else
    {
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
        qDebug() << QDateTime::currentDateTime() << "ThumbnailItemDelegate::updateThumbnail" << "insert to dirs"  << index.internalId() << thumb_info.info.getFilePath();
#endif
        QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
        icon = QIcon(QPixmap::fromImage(PictureLoader::styleThumbnail(icon.pixmap(icon.availableSizes().last()).toImage(), thumb_info)));
        m_thumbnails.insert(index.internalId(), icon);
    }
}

void ThumbnailItemDelegate::setThumbnailSize(const QSize &size)
{
    m_thumb_size = size;
}

void ThumbnailItemDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

//    qDebug() << index << index.data();
    if (m_thumbnails.contains(index.internalId()))
    {
        if (QStyleOptionViewItemV4 *v4 = qstyleoption_cast<QStyleOptionViewItemV4 *>(option))
        {
            v4->icon = m_thumbnails.value(index.internalId(), QIcon());
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
