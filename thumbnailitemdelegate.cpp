#include "thumbnailitemdelegate.h"

#include <QtConcurrentRun>
#include <QDebug>

ThumbnailItemDelegate::ThumbnailItemDelegate(ThumbnailInfo thumb_info, const QModelIndex &index, QObject *parent) :
    QItemDelegate(parent),
    m_thumb_info(thumb_info),
    m_index(index)
{
    m_watcherThumbnail = new QFutureWatcher<QImage>(this);
    connect(m_watcherThumbnail, SIGNAL(resultReadyAt(int)), this, SLOT(showThumbnail(int)));

    m_watcherThumbnail->setFuture(QtConcurrent::run(PictureLoader::getThumbnail, thumb_info));
}

QSize ThumbnailItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!m_thumb_info.thumbSize.isEmpty())
    {
        QSize size_grid = QSize(m_thumb_info.thumbSize.width() + 50, m_thumb_info.thumbSize.height() + 50);
        return size_grid;
    }
    else
    {
        return QItemDelegate::sizeHint(option, index);
    }
}

void ThumbnailItemDelegate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap) const
{
    painter->save();

    if (!m_thumbnail.isNull())
    {
        QPixmap thumbPixmap = QPixmap::fromImage(m_thumbnail);
        QPoint p = QStyle::alignedRect(option.direction, Qt::AlignHCenter | Qt::AlignTop,
                                       m_thumb_info.thumbSize, painter->clipBoundingRect().toRect()).topLeft();
        if (option.state & QStyle::State_Selected) {
            QPixmap *pm = selected(thumbPixmap, option.palette, option.state & QStyle::State_Enabled);
            painter->drawPixmap(p, *pm);
        } else {
            painter->drawPixmap(p, thumbPixmap);
        }
    }
    else
    {
        QItemDelegate::drawDecoration(painter, option, rect, pixmap);
    }
    painter->restore();
}

void ThumbnailItemDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const
{
    QRect myrect = rect;
    myrect.moveTop(painter->clipBoundingRect().y() +  m_thumb_info.thumbSize.height() + 2);
    QItemDelegate::drawDisplay(painter, option, myrect, text);
}

void ThumbnailItemDelegate::drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    QRect myrect = rect;
    myrect.moveTop(painter->clipBoundingRect().y() +  m_thumb_info.thumbSize.height() + 2);
    QItemDelegate::drawFocus(painter, option, myrect);
}

void ThumbnailItemDelegate::showThumbnail(int num)
{
    m_thumbnail = m_watcherThumbnail->resultAt(num);
    m_watcherThumbnail->setFuture(QFuture<QImage>());
    emit thumbnailFinished(m_index);
}
