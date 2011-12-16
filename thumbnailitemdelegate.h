#ifndef THUMBNAILITEMDELEGATE_H
#define THUMBNAILITEMDELEGATE_H

#include "picture_loader.h"

#include <QItemDelegate>
#include <QModelIndex>
#include <QPainter>
#include <QFutureWatcher>

class ThumbnailItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ThumbnailItemDelegate(ThumbnailInfo thumb_info, const QModelIndex &index, QObject *parent = 0);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap) const;
    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const;
    void drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;

private:
    ThumbnailInfo m_thumb_info;
    QImage m_thumbnail;
    QModelIndex m_index;
    QFutureWatcher<QImage> *m_watcherThumbnail;

signals:
    void thumbnailFinished(QModelIndex);

private slots:
    void showThumbnail(int num);
};

#endif // THUMBNAILITEMDELEGATE_H
