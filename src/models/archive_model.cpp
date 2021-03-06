#include "models/archive_model.h"

#include <QApplication>
#include <QFileIconProvider>
#include <QFileSystemModel>
#include <QStyle>

#include "helper.h"
#include "models/archive_item.h"
//#define DEBUG_MODEL_FILES
#ifdef DEBUG_MODEL_FILES
#include "helper.h"
#endif

ArchiveModel::ArchiveModel(const std::vector<ArchiveFileInfo> &archive_files,
                           QObject *const parent)
    : QAbstractItemModel(parent)
    , rootItem(new ArchiveItem("", QDateTime(), 0, "",
                               NodeType::Archive))
    , m_icon_dir(QApplication::style()->standardIcon(QStyle::SP_DirIcon))
    , m_icon_file(QIcon::fromTheme("image-x-generic"))
{
#ifdef DEBUG_MODEL_FILES
    DEBUGOUT << path;
#endif
    if (m_icon_file.isNull())
    {
        m_icon_file = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    }
    populate(archive_files);
}

void ArchiveModel::populate(const std::vector<ArchiveFileInfo> &archive_files)
{
    /* Populate model */
    ArchiveItem *rootArchiveItem = rootItem;

    for (const ArchiveFileInfo &currentArchiveFile : archive_files)
    {
#ifdef DEBUG_MODEL_FILES
        DEBUGOUT << currentArchiveFile.name;
#endif
        ArchiveItem *node = rootArchiveItem;
        const QStringList file_path_parts = currentArchiveFile.m_name.split('/');
        QString folderPath = "/";
        for (int j = 0; j < file_path_parts.size(); ++j)
        {
            const QString currentFilePathPart = file_path_parts.at(j);
            if (currentFilePathPart.size() > 0)
            {
                folderPath.append(currentFilePathPart + "/");
                if (j < file_path_parts.size() - 1)
                {
                    node = AddNode(currentFilePathPart,
                                   currentArchiveFile.m_dateTime,
                                   0,
                                   folderPath,
                                   NodeType::Directory,
                                   node);
                }
                else
                {
                    const QFileInfo fi(currentArchiveFile.m_name);
                    if (Helper::isImageFileExtension(fi))
                    {
                        const QString nodeFilePath = currentArchiveFile.m_name;
                        node = AddNode(currentFilePathPart,
                                       currentArchiveFile.m_dateTime,
                                       currentArchiveFile.m_uncompressedSize,
                                       nodeFilePath,
                                       NodeType::Image,
                                       node);
                    }
                }
            }
        }
    }
}

ArchiveModel::~ArchiveModel()
{
    delete rootItem;
}

int ArchiveModel::columnCount(const QModelIndex & /* parent */) const
{
    return rootItem->columnCount();
}


QVariant ArchiveModel::data(const QModelIndex &index, const int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const ArchiveItem *const item = getItem(index);

    return item->data(role, index.column());
}

Qt::ItemFlags ArchiveModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

ArchiveItem *ArchiveModel::getItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        ArchiveItem *item = static_cast<ArchiveItem*>(index.internalPointer());
        if (item)
        {
            return item;
        }
    }
    return rootItem;
}

QModelIndex ArchiveModel::createIndexMine(int arow, int acolumn, quintptr i) const
{
    return createIndex(arow, acolumn, i);
}

QString ArchiveModel::getIndexIdentifier(const QModelIndex &index) const
{
    Q_ASSERT(this == index.model());
    const ArchiveItem* item = static_cast<ArchiveItem*>(index.internalPointer());
    return item->getPath();
}

QVariant ArchiveModel::headerData(const int section,
                                  const Qt::Orientation orientation,
                                  const int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return QFileSystemModel::tr("Name");
        case 1:
            return QFileSystemModel::tr("Size");
        case 2:
            return QFileSystemModel::tr("Date Modified");
        default:
            Q_ASSERT(false);
        }
    }

    return QVariant();
}

QModelIndex ArchiveModel::index(const int row,
                                const int column,
                                const QModelIndex &parent) const
{
    if (column < 0 || row < 0)
    {
        return QModelIndex();
    }

    if (parent.isValid() && parent.column() != 0)
    {
        return QModelIndex();
    }

    const ArchiveItem *const parentItem = getItem(parent);

    ArchiveItem *const childItem = parentItem->child(row);
    if (childItem)
    {
        return createIndex(row, column, childItem);
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex ArchiveModel::parent(const QModelIndex &index) const
{
    if (!index.isValid() || rowCount() == 0)
    {
        return QModelIndex();
    }

    const ArchiveItem *const childItem = getItem(index);
    if (childItem == rootItem)
    {
        return QModelIndex();
    }
    ArchiveItem *const parentItem = childItem->parent();

    if (parentItem == rootItem)
    {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int ArchiveModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
    {
        return 0;
    }

    const ArchiveItem *const parentItem = getItem(parent);

    return parentItem->childCount();
}

ArchiveItem *ArchiveModel::AddNode(
        const QString &name,
        const QDateTime &date,
        const qint64 &bytes,
        const QString &path,
        const NodeType type,
        ArchiveItem *const parent)
{
    for (ArchiveItem *const sibling : parent->children())
    {
        if (sibling->data(Qt::EditRole, ArchiveItem::col_name) == name)
        {
            return sibling;
        }
    }

    ArchiveItem *ntvi = new ArchiveItem(name,
                                        date,
                                        bytes,
                                        path,
                                        type,
                                        (type == NodeType::Image
                                         ? m_icon_file : m_icon_dir),
                                        parent);
    parent->appendChild(ntvi);

    return ntvi;
}

QModelIndex ArchiveModel::getIndexFromPath(const QString &path) const
{
    QModelIndex cri = createIndex(0, 0, rootItem);

    const QStringList file_path_parts = path.split('/',
                                                   Qt::SkipEmptyParts);
    for (const QString &file_path_part : file_path_parts)
    {
        cri = findIndexChild(file_path_part, cri);
        if (!cri.isValid())
        {
            return QModelIndex();
        }
    }

    return cri;
}

QModelIndex ArchiveModel::findIndexChild(const QString &text,
                                         const QModelIndex &root) const
{
    if (!root.isValid()) { return QModelIndex(); }
    int i = 0;
    while (true)
    {
        const QModelIndex currentIndex = index(i, 0, root);
        if (!currentIndex.isValid())
        {
            break;
        }
        const QString childText = currentIndex.data(Qt::DisplayRole).toString();
        if (childText == text)
        {
            return currentIndex;
        }
    }
    return QModelIndex();
}
