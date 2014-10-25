#include "kiv/src/widgets/files_view/thumbnailitemdelegate.h"

#include <QAbstractItemView>
#include <QCryptographicHash>
#include <QDir>
#include <QFileIconProvider>
#include <QFileSystemModel>
#include <QPixmap>
#ifdef QT5
#include <QtConcurrent/QtConcurrentMap>
#include <QtConcurrent/QtConcurrentRun>
#else
#include <QtConcurrentMap>
#include <QtConcurrentRun>
#endif

//#define DEBUG_THUMBNAIL_ITEM_DELEGATE
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
#include "kiv/src/helper.h"
#endif


ThumbnailItemDelegate::ThumbnailItemDelegate(
        const IPictureLoader *const picture_loader,
        const QSize &thumbSize,
        QObject *parent)
    : QStyledItemDelegate(parent)
    , m_thumb_size(thumbSize)
    , m_watcherThumbnail(new QFutureWatcher<QImage>(this))
    , m_picture_loader(picture_loader)
{
    connect(m_watcherThumbnail, SIGNAL(resultReadyAt(int)),
            this, SLOT(showThumbnail(int)));
}

ThumbnailItemDelegate::~ThumbnailItemDelegate()
{
    clearThumbnailsCache();
}

void ThumbnailItemDelegate::setThumbnailSize(const QSize &size)
{
    if (size != m_thumb_size)
    {
        m_thumb_size = size;
        clearThumbnailsCache();
    }
}

QSize ThumbnailItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                      const QModelIndex &/*index*/) const
{
    const QSize &size_grid = QSize(
                m_thumb_size.width() + 6,
                m_thumb_size.height() + option.decorationSize.height());
    return size_grid;
}

void ThumbnailItemDelegate::cancelThumbnailGeneration()
{
    if (!m_files_to_process.isEmpty())
    {
        ProcessInfo *oldIndex = m_files_to_process.at(0);
        m_files_to_process.clear();
        m_files_to_process.append(oldIndex);
    }
}

void ThumbnailItemDelegate::updateThumbnail(const FileInfo &info,
                                            const QModelIndex &index)
{
    QDateTime currentDateTime;
    const QByteArray &filepath =
            index.data(QFileSystemModel::FilePathRole).toByteArray();
    const QByteArray &path_hash =
            QCryptographicHash::hash(filepath, QCryptographicHash::Md4);
    const QVariant &date = index.data(Helper::ROLE_FILE_DATE);

#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
    DEBUGOUT << filepath;
#endif

    if (date.isValid())
    {
        currentDateTime = date.toDateTime();
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
        DEBUGOUT << "Date from QVariant";
#endif
    }
    else
    {
        if (const QFileSystemModel *fsm =
            qobject_cast<const QFileSystemModel *>(index.model()))
        {
            currentDateTime = fsm->lastModified(index);
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
            DEBUGOUT << "Date from QFileSystemModel";
#endif
        }
        else
        {
            QFileInfo fi(filepath);
            currentDateTime = fi.lastModified();
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
            DEBUGOUT << "Date from QFileInfo";
#endif
        }
    }

#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
    DEBUGOUT << currentDateTime;
#endif

    if (m_thumbnails.contains(path_hash))
    {
        const ThumbImageDate *ti = m_thumbnails.value(path_hash);
        if (ti->getDate() == currentDateTime)
        {
            return;
        }
    }

#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
    DEBUGOUT << "generating for" << filepath;
#endif

    if (info.fileExists())
    {
        m_files_to_process.append(new ProcessInfo(index, info, path_hash,
                                                  currentDateTime));
        if (m_files_to_process.size() == 1)
        {
            m_watcherThumbnail->setFuture(
                        QtConcurrent::run(m_picture_loader,
                                          &IPictureLoader::getThumbnail,
                                          ThumbnailInfo(info, m_thumb_size)));
        }
    }
    else
    {
        QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
        QImage thumb = m_picture_loader->styleThumbnail(
                    icon.pixmap(icon.availableSizes().last()).toImage(),
                    m_thumb_size);
        icon = QIcon(QPixmap::fromImage(thumb));

        m_thumbnails.insert(path_hash, new ThumbImageDate(icon,
                                                          currentDateTime));
        emit thumbnailFinished(index);
    }
}


void ThumbnailItemDelegate::initStyleOption(QStyleOptionViewItem *option,
                                            const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

    const QByteArray &path_hash = QCryptographicHash::hash(
                index.data(QFileSystemModel::FilePathRole).toByteArray(),
                QCryptographicHash::Md4);
    if (m_thumbnails.contains(path_hash))
    {
        if (QStyleOptionViewItemV4 *v4 =
            qstyleoption_cast<QStyleOptionViewItemV4 *>(option))
        {
            const ThumbImageDate *tid = m_thumbnails.value(path_hash);
            if (tid->getThumb().availableSizes().isEmpty())
            {
                return;
            }
            v4->icon = tid->getThumb();
            v4->decorationSize = v4->icon.availableSizes().last();
        }
    }
}

void ThumbnailItemDelegate::clearThumbnailsCache()
{
    qDeleteAll(m_files_to_process);
    m_files_to_process.clear();
    qDeleteAll(m_thumbnails);
    m_thumbnails.clear();
}

void ThumbnailItemDelegate::showThumbnail(int num)
{
    if (m_files_to_process.isEmpty())
    {
        m_watcherThumbnail->setFuture(QFuture<QImage>());
        return;
    }
#ifdef DEBUG_THUMBNAIL_ITEM_DELEGATE
    DEBUGOUT << m_files_to_process.at(0)->getFileInfo().getPath();
#endif

    ThumbImageDate *tid = new ThumbImageDate(
                QIcon(QPixmap::fromImage(m_watcherThumbnail->resultAt(num))),
                m_files_to_process.first()->getDate());
    m_thumbnails.insert(m_files_to_process.first()->getPathHash(), tid);

    emit thumbnailFinished(m_files_to_process.first()->getIndex());


    delete m_files_to_process.first();
    m_files_to_process.removeFirst();

    if (!m_files_to_process.isEmpty())
    {
        m_watcherThumbnail->setFuture(
                    QtConcurrent::run(
                        m_picture_loader,
                        &IPictureLoader::getThumbnail,
                        ThumbnailInfo(m_files_to_process.at(0)->getFileInfo(),
                                      m_thumb_size)));
    }
    else
    {
        m_files_to_process.clear();
        m_watcherThumbnail->setFuture(QFuture<QImage>());
    }

}
