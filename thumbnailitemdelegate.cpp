#include "thumbnailitemdelegate.h"

#include <QtConcurrentRun>
#include <QFileIconProvider>
#include <QPixmap>

ThumbnailItemDelegate::ThumbnailItemDelegate(ThumbnailInfo thumb_info, const QModelIndex &index, QObject *parent) :
    QStyledItemDelegate(parent),
    m_thumb_info(thumb_info),
    m_index(index)
{
    if (m_thumb_info.info.fileExists())
    {
        m_watcherThumbnail = new QFutureWatcher<QImage>(this);
        connect(m_watcherThumbnail, SIGNAL(resultReadyAt(int)), this, SLOT(showThumbnail(int)));

        m_watcherThumbnail->setFuture(QtConcurrent::run(PictureLoader::getThumbnail, thumb_info));
    }
    else
    {
        QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
        m_thumbnail = QIcon(QPixmap::fromImage(PictureLoader::styleThumbnail(icon.pixmap(icon.availableSizes().last()).toImage(), thumb_info)));
    }
}

QSize ThumbnailItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size_grid = QSize(m_thumb_info.thumbSize.width() + 6, m_thumb_info.thumbSize.height() + option.fontMetrics.height() + 10);
    return size_grid;
}

void ThumbnailItemDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

    if (!m_thumbnail.isNull())
    {
        if (QStyleOptionViewItemV4 *v4 = qstyleoption_cast<QStyleOptionViewItemV4 *>(option))
        {
            v4->icon = m_thumbnail;
            v4->decorationSize = m_thumbnail.availableSizes().last();
        }
    }
}

void ThumbnailItemDelegate::showThumbnail(int num)
{
    m_thumbnail = QIcon(QPixmap::fromImage(m_watcherThumbnail->resultAt(num)));
    m_watcherThumbnail->setFuture(QFuture<QImage>());
    emit thumbnailFinished(m_index);
}
