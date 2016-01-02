#include "modelwrapper.h"

#include <QFileSystemModel>

#include "kiv/src/helper.h"
#include "kiv/src/models/archive_model.h"


ArchiveModelWrapper::ArchiveModelWrapper(ArchiveModel *model)
    : m_model(model)
{}

NodeType ArchiveModelWrapper::getNodeType(const QModelIndex &index) const
{
    const ArchiveItem *ai = m_model->getItem(index);
    return ai->getType();
}

QAbstractItemModel *ArchiveModelWrapper::getModel()
{ return m_model; }

QModelIndex ArchiveModelWrapper::getContainer(const FileInfo &info) const
{ return m_model->getDirectory(info.getArchiveContainerPath()); }

QModelIndex ArchiveModelWrapper::getFile(const FileInfo &info, const QModelIndex& container) const
{ return m_model->findIndexChild(info.getImageFileName(), container); }

QString ArchiveModelWrapper::filePath(const QModelIndex &index) const
{ return index.data(QFileSystemModel::FilePathRole).toString(); }

bool ArchiveModelWrapper::isDir(const QModelIndex &index) const
{ return getNodeType(index) == NodeType::Directory; }

QFileInfo ArchiveModelWrapper::fileInfo(const QModelIndex &index) const
{ return QFileInfo(this->filePath(index)); }




FileSystemModelWrapper::FileSystemModelWrapper(FileSystemModel *model)
    : m_model(model)
{}

NodeType FileSystemModelWrapper::getNodeType(const QModelIndex &index) const
{
    const QFileInfo indexFileInfo = m_model->fileInfo(index);
    if (Helper::isImageFile(indexFileInfo))
    {
        return NodeType::Image;
    }
    else if (Helper::isArchiveFile(indexFileInfo))
    {
        return NodeType::Archive;
    }
    else
    {
        return NodeType::Directory;
    }
}

QAbstractItemModel *FileSystemModelWrapper::getModel()
{ return m_model; }

QModelIndex FileSystemModelWrapper::getContainer(const FileInfo &info) const
{ return m_model->index(info.getContainerPath()); }

QModelIndex FileSystemModelWrapper::getFile(const FileInfo &info, const QModelIndex& /*container*/) const
{ return m_model->index(info.getPath()); }

QString FileSystemModelWrapper::filePath(const QModelIndex &index) const
{ return m_model->filePath(index); }

bool FileSystemModelWrapper::isDir(const QModelIndex &index) const
{ return m_model->isDir(index); }

QFileInfo FileSystemModelWrapper::fileInfo(const QModelIndex &index) const
{ return m_model->fileInfo(index); }



bool FileListSortFilterProxyModel::lessThan(
        const QModelIndex &left, const QModelIndex &right) const
{
    const ArchiveModel *am = qobject_cast<const ArchiveModel*>(left.model());
    if (am)
    {
        const ArchiveItem *left_item = am->getItem(left);
        const ArchiveItem *right_item = am->getItem(right);
        const NodeType left_type = left_item->getType();
        const NodeType right_type = right_item->getType();

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
	    default:
	      Q_ASSERT(false);
	      break;
            }
        }

        if (left_type == NodeType::Directory)
        {
            return (this->sortOrder() == Qt::AscendingOrder);
        }
        else if (right_type == NodeType::Directory)
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
	    default:
	      break;
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
    const ArchiveModel *am = qobject_cast<const ArchiveModel*>(this->sourceModel());
    Q_ASSERT(am);
    return (am->getItem(index0)->getType() != NodeType::Image);
}
