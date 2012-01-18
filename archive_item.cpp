#include <QStringList>

#include "archive_item.h"
#include "helper.h"
#include <QFileSystemModel>
#include <QDebug>

ArchiveItem::ArchiveItem(const QString &name, const QDateTime &date, const quint64 &bytes, const QString &path, int type, const QIcon &icon, ArchiveItem *parent) :
    childItems(QList<ArchiveItem*>()),
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

}

ArchiveItem::~ArchiveItem()
{
    qDeleteAll(childItems);
}

void ArchiveItem::clear()
{
    while (childCount() > 0)
    {
        childItems.at(0)->clear();
        delete childItems.at(0);
        childItems.removeFirst();
    }
//    qDeleteAll(childItems);
    childItems.clear();
}

void ArchiveItem::appendChild(ArchiveItem *item)
{
    childItems.append(item);
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
