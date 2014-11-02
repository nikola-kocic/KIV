#ifndef THUMBNAILITEMDELEGATE_H
#define THUMBNAILITEMDELEGATE_H

#include <QDateTime>
#include <QFutureWatcher>
#include <QModelIndex>
#include <QPainter>
#include <QStyledItemDelegate>

#include "kiv/include/IPictureLoader.h"
#include "kiv/src/helper.h"

class ThumbnailItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ThumbnailItemDelegate(
            const IPictureLoader *const picture_loader,
            const QSize &thumbSize,
            QObject *parent = 0);
    ~ThumbnailItemDelegate() override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
    void updateThumbnail(const FileInfo &info, const QModelIndex &index);
    void setThumbnailSize(const QSize &size);
    void cancelThumbnailGeneration();

protected:
    void initStyleOption(QStyleOptionViewItem *option,
                         const QModelIndex &index) const override;

private:

    class ThumbImageDate
    {
    public:
        ThumbImageDate(const QIcon &thumb, const QDateTime &date)
            : m_thumb(thumb)
            , m_date(date)
        {}

        inline QDateTime getDate() const { return m_date; }
        inline QIcon getThumb() const { return m_thumb; }

    private:
        const QIcon m_thumb;
        const QDateTime m_date;
    };

    class ProcessInfo
    {
    public:
        ProcessInfo(const QModelIndex &index,
                    const FileInfo &fileinfo,
                    const QByteArray &pathhash,
                    const QDateTime &date)
            : m_index(index)
            , m_fileinfo(fileinfo)
            , m_path_hash(pathhash)
            , m_date(date)
        {}

        inline QDateTime getDate() const { return m_date; }
        inline QModelIndex getIndex() const { return m_index; }
        inline FileInfo getFileInfo() const { return m_fileinfo; }
        inline QByteArray getPathHash() const { return m_path_hash; }

    private:
        const QModelIndex m_index;
        const FileInfo m_fileinfo;
        const QByteArray m_path_hash;
        const QDateTime m_date;
    };

    void clearThumbnailsCache();

    QSize m_thumb_size;
    QFutureWatcher<QImage> *m_watcherThumbnail;
    QHash<QByteArray, ThumbImageDate *> m_thumbnails;

    QList<ProcessInfo *> m_files_to_process;
    const IPictureLoader *const m_picture_loader;

signals:
    void thumbnailFinished(QModelIndex);

private slots:
    void showThumbnail(int num);
};

#endif  // THUMBNAILITEMDELEGATE_H
