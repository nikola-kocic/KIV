#ifndef THUMBNAILITEMDELEGATE_H
#define THUMBNAILITEMDELEGATE_H

#include "picture_loader.h"

#include <QModelIndex>
#include <QPainter>
#include <QFutureWatcher>
#include <QStyledItemDelegate>

class ThumbnailItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ThumbnailItemDelegate(QSize thumbSize, QObject *parent);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void updateThumbnails(ThumbnailInfo thumb_info);

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const;

private:
    QSize m_thumb_size;
    QHash<QModelIndex, QIcon> m_thumbnails;
    QFutureWatcher<QImage> *m_watcherThumbnail;

    QList<ThumbnailInfo> m_files;
    QList<QModelIndex> m_indexes;
    int m_thumbs_return_count;

signals:
    void thumbnailFinished(QModelIndex);

private slots:
    void showThumbnail(int num);
};

#endif // THUMBNAILITEMDELEGATE_H
