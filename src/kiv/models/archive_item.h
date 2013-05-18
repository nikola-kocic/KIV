#ifndef ARCHIVE_ITEM_H
#define ARCHIVE_ITEM_H

#include <QDateTime>
#include <QIcon>
#include <QVariant>

class ArchiveItem
{
public:
    explicit ArchiveItem(const QString &name, const QDateTime &date, const quint64 &bytes, const QString &path, const int type, const QIcon &icon = QIcon(), ArchiveItem *parent = 0);
    ~ArchiveItem();
    void appendChild(ArchiveItem *child);
    ArchiveItem *child(int row);
    int childCount() const;
    static int columnCount();
    QVariant data(int role, int column) const;
    int row() const;
    ArchiveItem *parent();
    void setDate(const QDateTime &date);
    void setSize(const quint64 &bytes);

    static const int col_name = 0;
    static const int col_size = 1;
    static const int col_date = 2;

    static const int TYPE_ARCHIVE = 1;
    static const int TYPE_ARCHIVE_DIR = 2;
    static const int TYPE_ARCHIVE_FILE = 3;

private:
    int indexToInsertByName(const ArchiveItem* const item);
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