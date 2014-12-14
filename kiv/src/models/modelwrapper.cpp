#include "modelwrapper.h"

#include <QFileSystemModel>

#include "kiv/src/helper.h"
#include "kiv/src/models/archive_model.h"

bool FileListSortFilterProxyModel::lessThan(
        const QModelIndex &left, const QModelIndex &right) const
{
    if (qobject_cast<const ArchiveModel*>(left.model()))
    {
        const int left_type = left.data(Helper::ROLE_TYPE).toInt();
        const int right_type = right.data(Helper::ROLE_TYPE).toInt();

        const QString left_name =
                left.sibling(left.row(),ArchiveItem::col_name).
                data(Qt::EditRole).toString();
        const QString right_name =
                right.sibling(right.row(), ArchiveItem::col_name).
                data(Qt::EditRole).toString();

        // if both indexes are same type, compare them
        if (left_type == right_type)
        {
            switch (this->sortColumn())
            {
            case ArchiveItem::col_name:
            {
                return (Helper::naturalCompare(left_name, right_name,
                                               Qt::CaseInsensitive)
                        < 0);
            }
            case ArchiveItem::col_date:
            {
                const QDateTime left_date =
                        left.data(Qt::EditRole).toDateTime();
                const QDateTime right_date =
                        right.data(Qt::EditRole).toDateTime();

                if (left_date < right_date)
                {
                    return true;
                }

                else if (left_date > right_date)
                {
                    return false;
                }
                else
                {
                    return (Helper::naturalCompare(left_name, right_name,
                                                   Qt::CaseInsensitive)
                            < 0);
                }
            }
            case ArchiveItem::col_size:
            {
                const qint64 left_size = left.data(Qt::EditRole).toLongLong();
                const qint64 right_size = right.data(Qt::EditRole).toLongLong();
                if (left_size < right_size)
                {
                    return true;
                }

                else if (left_size > right_size)
                {
                    return false;
                }
                else
                {
                    // If same size, sort by Name Ascending
                    const int name_comparison = Helper::naturalCompare(
                                left_name, right_name, Qt::CaseInsensitive);
                    if (this->sortOrder() == Qt::AscendingOrder)
                        return (name_comparison < 0);
                    else
                        return (name_comparison > 0);
                }
            }
            }
        }

        if (left_type == ArchiveItem::TYPE_ARCHIVE_DIR)
        {
            return (this->sortOrder() == Qt::AscendingOrder);
        }
        else if (right_type == ArchiveItem::TYPE_ARCHIVE_DIR)
        {
            return (this->sortOrder() == Qt::DescendingOrder);
        }

        return true;
    }
    else if (const QFileSystemModel *fsm =
             qobject_cast<const QFileSystemModel*>(left.model()))
    {
        const QFileInfo left_fileinfo = fsm->fileInfo(left);
        const QFileInfo right_fileinfo = fsm->fileInfo(right);

        const bool sametype = (
                    (left_fileinfo.isDir() && right_fileinfo.isDir()) ||
                    (!left_fileinfo.isDir() && !right_fileinfo.isDir()));

        if (sametype)  // if both indexes are same type, compare them
        {
            switch (this->sortColumn()) // 0:displayName, 1:size, 2:type, 3:time
            {
            case 0:
            {
                return (Helper::naturalCompare(
                            left_fileinfo.fileName(), right_fileinfo.fileName(),
                            Qt::CaseInsensitive)
                        < 0);
            }
            case 3:
            {
                if (left_fileinfo.lastModified()
                    < right_fileinfo.lastModified())
                {
                    return true;
                }

                else if (left_fileinfo.lastModified()
                         > right_fileinfo.lastModified())
                {
                    return false;
                }
                else
                {
                    return (Helper::naturalCompare(
                                left_fileinfo.fileName(),
                                right_fileinfo.fileName(),
                                Qt::CaseInsensitive)
                            < 0);
                }
            }
            case 1:
            {
                if (left_fileinfo.size() < right_fileinfo.size())
                {
                    return true;
                }

                else if (left_fileinfo.size() > right_fileinfo.size())
                {
                    return false;
                }
                else
                {
                    // If same size, sort by Name Ascending
                    const int name_comparison = Helper::naturalCompare(
                                left_fileinfo.fileName(),
                                right_fileinfo.fileName(),
                                Qt::CaseInsensitive);
                    if (this->sortOrder() == Qt::AscendingOrder)
                    {
                        return (name_comparison < 0);
                    }
                    else
                    {
                        return (name_comparison > 0);
                    }
                }
            }
            }
        }

        if (left_fileinfo.isDir())
        {
            return (this->sortOrder() == Qt::AscendingOrder);
        }
        else if (right_fileinfo.isDir())
        {
            return (this->sortOrder() == Qt::DescendingOrder);
        }

        return true;

    }

    return QSortFilterProxyModel::lessThan(left, right);

}

bool ContainersSortFilterProxyModel::filterAcceptsColumn(
        int source_column, const QModelIndex &/*source_parent*/) const
{
    return (source_column == 0);
}

bool ContainersSortFilterProxyModel::filterAcceptsRow(
        int source_row, const QModelIndex &source_parent) const
{
    if (const QFileSystemModel *fsm =
        qobject_cast<const QFileSystemModel*>(this->sourceModel()))
    {
        const QFileInfo fi = fsm->fileInfo(fsm->index(source_row, 0,
                                                      source_parent));
        return (fi.isDir() || Helper::isArchiveFile(fi));
    }
    return true;
}

bool ArchiveDirsSortFilterProxyModel::filterAcceptsColumn(
        int source_column, const QModelIndex &/*source_parent*/) const
{
    return (source_column == 0);
}

bool ArchiveDirsSortFilterProxyModel::filterAcceptsRow(
        int sourceRow, const QModelIndex &sourceParent) const
{
    const QModelIndex index0 = this->sourceModel()->index(sourceRow, 0,
                                                          sourceParent);
    return !(index0.data(Helper::ROLE_TYPE).toInt()
             == ArchiveItem::TYPE_ARCHIVE_FILE);
}



ModelWrapper::ModelWrapper(QObject *parent) : QObject(parent)
{

}

ModelWrapper::~ModelWrapper()
{

}

