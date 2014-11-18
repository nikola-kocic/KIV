#include "kiv/src/models/archive_item.h"

#include <QFileSystemModel>
#include <QStringList>

#include "kiv/src/helper.h"

//#define DEBUG_ARCHIVE_ITEM
#ifdef DEBUG_ARCHIVE_ITEM
#include "kiv/src/helper.h"
#endif

ArchiveItem::ArchiveItem(const QString &name,
                         const QDateTime &date,
                         const quint64 &bytes,
                         const QString &path,
                         const int type,
                         const QIcon &icon,
                         ArchiveItem *const parent)
    : parentItem(parent)
    , m_name(name)
    , m_date(date)
    , m_bytes(bytes)
    , m_path(path)
    , m_type(type)
    , m_icon(icon)
    , m_tooltip(QFileSystemModel::tr("Name") + ": " + m_name + "\n"
                + QFileSystemModel::tr("Date Modified") + ": "
                + m_date.toString(Qt::SystemLocaleShortDate))
{
    if (m_type == TYPE_ARCHIVE_FILE)
    {
        m_tooltip.append("\n" + QFileSystemModel::tr("Size") + ": "
                         + Helper::size(m_bytes));
    }

#ifdef DEBUG_ARCHIVE_ITEM
    DEBUGOUT << name << "handle" << this << "parent:" << parent;
#endif
}

ArchiveItem::~ArchiveItem()
{
    qDeleteAll(childItems);
#ifdef DEBUG_ARCHIVE_ITEM
    DEBUGOUT << m_name << "handle" << this;
#endif
}

void ArchiveItem::appendChild(ArchiveItem *const item)
{
    childItems.insert(indexToInsertByName(item), item);
}

int ArchiveItem::indexToInsertByName(const ArchiveItem* const item)
{
    const int type = item->data(Helper::ROLE_TYPE, 0).toInt();
    const QString itemName = item->data(Qt::EditRole, col_name).toString();
    int i = 0;

    for (; i < childCount(); ++i)
    {
        int currentItemType = child(i)->data(Helper::ROLE_TYPE, 0).toInt();
        if (type == TYPE_ARCHIVE)
        {
            // TODO: Allow archive in archive
            continue;
        }
        if (currentItemType == type)
        {
            const QString currentItemName =
                    child(i)->data(Qt::EditRole, col_name).toString();
            if (Helper::naturalCompare(currentItemName, itemName,
                                       Qt::CaseInsensitive)
                > 0)
            {
                return i;
            }
        }
        else if (type == TYPE_ARCHIVE_DIR)
            // Directories go above files
        {
            return i;
        }
    }

    return i;
}

ArchiveItem *ArchiveItem::child(const int row) const
{
    return childItems.value(row);
}

int ArchiveItem::childCount() const
{
    return childItems.size();
}

QList<ArchiveItem *> ArchiveItem::children() const
{
    return childItems;
}

int ArchiveItem::columnCount()
{
    return col_count;
}

QVariant ArchiveItem::data(const int role, const int column) const
{
    switch (role)
    {
    case Qt::EditRole:
    {
        switch (column)
        {
        case col_name:
            return m_name;

        case col_size:
            return m_bytes;

        case col_date:
            return m_date;
        }
    }
    case Qt::DisplayRole:
    {
        switch (column)
        {
        case col_name:
            return m_name;

        case col_size:
            return Helper::size(m_bytes);

        case col_date:
            return m_date.toString(Qt::SystemLocaleShortDate);
        }
    }

    case Qt::DecorationRole:
    {
        if (column == 0)
        {
            return m_icon;
        }

        return QVariant();
    }

    case Qt::ToolTipRole:
        return m_tooltip;

    case QFileSystemModel::FilePathRole:
        return m_path;

    case Helper::ROLE_TYPE:
        return m_type;

    case Helper::ROLE_FILE_DATE:
        return m_date;
    }

    return QVariant();
}

ArchiveItem *ArchiveItem::parent() const
{
    return parentItem;
}

int ArchiveItem::row() const
{
    if (parentItem)
    {
        return parentItem->childItems.indexOf(const_cast<ArchiveItem*>(this));
    }

    return 0;
}
