#include <QStringList>

#include "archive_item.h"
#include "helper.h"
#include <QFileSystemModel>

//#define DEBUG_ARCHIVE_ITEM
#ifdef DEBUG_ARCHIVE_ITEM
#include <QDebug>
#endif

ArchiveItem::ArchiveItem(const QString &name, const QDateTime &date, const quint64 &bytes, const QString &path, const int type, const QIcon &icon, ArchiveItem *parent) :
    parentItem(parent),
    m_name(name),
    m_date(date),
    m_bytes(bytes),
    m_path(path),
    m_type(type),
    m_icon(icon),
    m_tooltip(QFileSystemModel::tr("Name") + ": " + m_name + "\n" + QFileSystemModel::tr("Date Modified") + ": " + m_date.toString(Qt::SystemLocaleShortDate))
{
    if (m_type == Helper::TYPE_ARCHIVE_FILE)
    {
        m_tooltip.append("\n" + QFileSystemModel::tr("Size") + ": " + Helper::size(m_bytes));
    }

#ifdef DEBUG_ARCHIVE_ITEM
    qDebug() << "created ArchiveItem" << name << "handle" << this << "parent:" << parent;
#endif
}

ArchiveItem::~ArchiveItem()
{
#ifdef DEBUG_ARCHIVE_ITEM
    qDebug() << "deleted" << m_name << "handle" << this;
#endif
    qDeleteAll(childItems);
}

void ArchiveItem::appendChild(ArchiveItem *item)
{
    childItems.insert(indexToInsertByName(item), item);
}

int ArchiveItem::indexToInsertByName(ArchiveItem *item)
{
    const int type = item->data(Helper::ROLE_TYPE, 0).toInt();
    const QString name = item->data(Qt::EditRole, col_name).toString();
    int i = 0;

    for (; i < this->childCount(); ++i)
    {
        int currentItemType = this->child(i)->data(Helper::ROLE_TYPE, 0).toInt();
        if (type == Helper::TYPE_ARCHIVE_DIR)
        {
            if (currentItemType == type)
            {
                if (this->child(i)->data(Qt::EditRole, col_name).toString().compare(name) > 0)
                {
                    return i;
                }
            }
            else
            {
                return i;
            }
        }
        else if (type == Helper::TYPE_ARCHIVE_FILE)
        {
            if (currentItemType == type)
            {
                if (this->child(i)->data(Qt::EditRole, col_name).toString().compare(name) > 0)
                {
                    return i;
                }
            }
        }
    }

    return i;
}

ArchiveItem *ArchiveItem::child(int row)
{
    return childItems.value(row);
}

int ArchiveItem::childCount() const
{
    return childItems.size();
}

int ArchiveItem::columnCount() const
{
    return col_count;
}

QVariant ArchiveItem::data(int role, int column) const
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
        if (column == col_name)
            return m_icon;
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

ArchiveItem *ArchiveItem::parent()
{
    return parentItem;
}

int ArchiveItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<ArchiveItem*>(this));

    return 0;
}

void ArchiveItem::clear()
{
    for (int row = 0; row < this->childCount(); ++row)
    {
        delete childItems.takeAt(0);
    }
}
