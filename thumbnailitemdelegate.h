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
    explicit ThumbnailItemDelegate(const QSize &thumbSize, QObject *parent = 0);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void updateThumbnail(const FileInfo &info, const QModelIndex &index);
    void setThumbnailSize(const QSize &size);
    void cancelThumbnailGeneration();

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const;

private:
    class ThumbImageDate
    {
    public:
        QIcon thumb;
        QDateTime date;
    };

    class ProcessInfo
    {
    public:
        ProcessInfo(const QModelIndex &index, const FileInfo &fileinfo, const QByteArray &pathhash, const QDateTime &date )
            : m_index(index)
            , m_fileinfo(fileinfo)
            , m_path_hash(pathhash)
            , m_date(date)
        {}

        QDateTime getDate() const { return m_date; }
        QModelIndex getIndex() const { return m_index; }
        FileInfo getFileInfo() const { return m_fileinfo; }
        QByteArray getPathHash() const { return m_path_hash; }

    private:
        QModelIndex m_index;
        FileInfo m_fileinfo;
        QByteArray m_path_hash;
        QDateTime m_date;
    };

    QSize m_thumb_size;
    QFutureWatcher<QImage> *m_watcherThumbnail;
    QHash<QByteArray, ThumbImageDate> m_thumbnails;

    QList<ProcessInfo> m_files_to_process;

signals:
    void thumbnailFinished(QModelIndex);

private slots:
    void showThumbnail(int num);
};

#endif // THUMBNAILITEMDELEGATE_H
