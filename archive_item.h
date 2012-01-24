#ifndef ARCHIVE_ITEM_H
#define ARCHIVE_ITEM_H

#include <QVariant>
#include <QDateTime>
#include <QIcon>

class ArchiveItem
{
public:
    explicit ArchiveItem(const QString &name, const QDateTime &date, const quint64 &bytes, const QString &path, const int type, const QIcon &icon = QIcon(), ArchiveItem *parent = 0);
    ~ArchiveItem();
    void appendChild(ArchiveItem *child);
    ArchiveItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int role, int column) const;
    int row() const;
    ArchiveItem *parent();

    void clear();

    static const int col_name = 0;
    static const int col_size = 1;
    static const int col_date = 2;

private:
    int indexToInsertByName(ArchiveItem *item);
    QList<ArchiveItem*> childItems;
    ArchiveItem *parentItem;

    QString m_name;
    QDateTime m_date;
    quint64 m_bytes;
    QString m_path;
    int m_type;
    QIcon m_icon;
    QString m_tooltip;

    static const int col_count = 3;
};

#endif // ARCHIVE_ITEM_H
