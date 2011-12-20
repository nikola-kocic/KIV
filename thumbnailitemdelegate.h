#ifndef THUMBNAILITEMDELEGATE_H
#define THUMBNAILITEMDELEGATE_H

#include "picture_loader.h"

#include <QModelIndex>
#include <QPainter>
#include <QFutureWatcher>
#include <QStyledItemDelegate>
#include <QDateTime>

class ThumbnailItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ThumbnailItemDelegate(QSize thumbSize, QObject *parent = 0);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void updateThumbnail(FileInfo info, QModelIndex index);
    void setThumbnailSize(const QSize &size);

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const;

private:
    QSize m_thumb_size;
    QFutureWatcher<QImage> *m_watcherThumbnail;
    QHash<QByteArray, QIcon> m_thumbnails;
//    QHash<QByteArray, ThumbImageDate> m_thumbnails;

//    QList<ThumbnailInfo> m_files;
//    QList<QModelIndex> m_indexes;
//    int m_thumbs_return_count;

signals:
    void thumbnailFinished(QModelIndex);

private slots:
    void showThumbnail(int num);
};

#endif // THUMBNAILITEMDELEGATE_H
