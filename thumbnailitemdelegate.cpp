#include "thumbnailitemdelegate.h"

#include <QtConcurrentRun>
#include <QFileIconProvider>
#include <QPixmap>
#include <QAbstractItemView>
#include <QtConcurrentMap>

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

void ThumbnailItemDelegate::updateThumbnails(ThumbnailInfo thumb_info)
{
    if (m_watcherThumbnail->isRunning())
    {
        m_watcherThumbnail->cancel();
        m_watcherThumbnail->waitForFinished();
    }

    m_thumb_size = thumb_info.thumbSize;
    m_thumbs_return_count = 0;
    m_indexes.clear();
    m_files.clear();

    QAbstractItemView *aiv = qobject_cast<QAbstractItemView *>(this->parent());

    for (int i = 0; i < aiv->model()->rowCount(aiv->rootIndex()); ++i)
    {
        FileInfo pli_info;

        QModelIndex index = aiv->model()->index(i, 0, aiv->rootIndex());
        int type = aiv->model()->data(index, ROLE_TYPE).toInt();
        QString name = aiv->model()->data(index, Qt::DisplayRole).toString();
        pli_info = thumb_info.info;
        if (type == TYPE_FILE || type == TYPE_ARCHIVE_FILE)
        {
            if (!m_thumbnails.contains(index))
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
    qDebug() << QDateTime::currentDateTime() << "ThumbnailItemDelegate::updateThumbnails" << "insert to files"  << index << index.data();
#endif
                ThumbnailInfo ti(pli_info, thumb_info.thumbSize);
                m_indexes.append(index);
                m_files.append(ti);
            }
        }
        else
        {
            QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
            icon = QIcon(QPixmap::fromImage(PictureLoader::styleThumbnail(icon.pixmap(icon.availableSizes().last()).toImage(), thumb_info)));
            m_thumbnails.insert(index, icon);
        }
    }


    m_watcherThumbnail->setFuture(QtConcurrent::mapped(m_files, PictureLoader::getThumbnail));
}

void ThumbnailItemDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

//    qDebug() << index << index.data();
    if (m_thumbnails.contains(index))
    {
        if (QStyleOptionViewItemV4 *v4 = qstyleoption_cast<QStyleOptionViewItemV4 *>(option))
        {
            v4->icon = m_thumbnails.value(index, QIcon());
            v4->decorationSize = v4->icon.availableSizes().last();
        }
    }
}

void ThumbnailItemDelegate::showThumbnail(int num)
{
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
    qDebug() << QDateTime::currentDateTime() << "ThumbnailItemDelegate::showThumbnail" << m_indexes.at(num) << m_indexes.at(num).data();
#endif
    m_thumbnails.insert(m_indexes.at(num), QIcon(QPixmap::fromImage(m_watcherThumbnail->resultAt(num))));
    emit thumbnailFinished(m_indexes.at(num));
    ++m_thumbs_return_count;
    if (m_thumbs_return_count == m_files.size())
    {
        m_indexes.clear();
        m_files.clear();
        m_thumbs_return_count = 0;
        m_watcherThumbnail->setFuture(QFuture<QImage>());
    }
}
