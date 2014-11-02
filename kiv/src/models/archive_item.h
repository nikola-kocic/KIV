#ifndef ARCHIVE_ITEM_H
#define ARCHIVE_ITEM_H

#include <QDateTime>
#include <QIcon>
#include <QVariant>

class ArchiveItem
{
public:
    explicit ArchiveItem(const QString &name,
                         const QDateTime &date,
                         const quint64 &bytes,
                         const QString &path,
                         const int type,
                         const QIcon &icon = QIcon(),
                         ArchiveItem *const parent = nullptr);
    ~ArchiveItem();
    void appendChild(ArchiveItem *const child);
    ArchiveItem *child(const int row) const;
    int childCount() const;
    QList<ArchiveItem *> children() const;
    static int columnCount();
    QVariant data(const int role, const int column) const;
    int row() const;
    ArchiveItem *parent() const;

    static const int col_name = 0;
    static const int col_size = 1;
    static const int col_date = 2;

    static const int TYPE_ARCHIVE = 1;
    static const int TYPE_ARCHIVE_DIR = 2;
    static const int TYPE_ARCHIVE_FILE = 3;

private:
    int indexToInsertByName(const ArchiveItem* const item);
    QList<ArchiveItem*> childItems;
    ArchiveItem *const parentItem;

    const QString m_name;
    const QDateTime m_date;
    const quint64 m_bytes;
    const QString m_path;
    const int m_type;
    const QIcon m_icon;
    QString m_tooltip;

    static const int col_count = 3;
};

#endif  // ARCHIVE_ITEM_H
