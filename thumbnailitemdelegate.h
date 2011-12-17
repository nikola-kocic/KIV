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
    explicit ThumbnailItemDelegate(ThumbnailInfo thumb_info, const QModelIndex &index, QObject *parent = 0);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const;

private:
    ThumbnailInfo m_thumb_info;
    QIcon m_thumbnail;
    QModelIndex m_index;
    QFutureWatcher<QImage> *m_watcherThumbnail;

signals:
    void thumbnailFinished(QModelIndex);

private slots:
    void showThumbnail(int num);
};

#endif // THUMBNAILITEMDELEGATE_H
