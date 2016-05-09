#ifndef ENUMS_H
#define ENUMS_H

#include <QDateTime>
#include <QString>

enum class LockMode: int
{
    None = 0,
    Autofit = 1,
    FitWidth = 2,
    FitHeight = 3,
    Zoom = 4,
};

struct MiddleClickAction
{
    static const int None = 0;
    static const int Fullscreen = 1;
    static const int AutoFit = 2;
    static const int ZoomReset = 3;
    static const int NextPage = 4;
    static const int Quit = 5;
    static const int Boss = 6;
};

struct WheelAction
{
    static const int None = 0;
    static const int Scroll = 1;
    static const int ChangePage = 2;
    static const int Zoom = 3;
};

enum class FileViewMode: int
{
    List = 0,
    Details = 1,
    Icons = 2,
};


enum class Column
{
    Name,
    Date,
    Size,
};

enum class SortOrder
{
    Asc,
    Desc,
};

enum class NodeType
{
    Directory,
    Image,
    Archive,
};

class ColumnSort {
public:
    ColumnSort(Column column, SortOrder order)
        : m_column(column)
        , m_order(order)
    {}
    Column getColumn() const { return m_column; }
    SortOrder getOrder() const { return m_order; }

    bool operator ==(const ColumnSort &b) const
    {
        return this->m_column == b.m_column && this->m_order == b.m_order;
    }
private:
    Column m_column;
    SortOrder m_order;
};

struct ArchiveFileInfo {
    /// File name.
    const QString m_name;
//    /// Version created by.
//    quint16 versionCreated;
//    /// Version needed to extract.
//    quint16 versionNeeded;
//    /// General purpose flags.
//    quint16 flags;
//    /// Compression method.
//    quint16 method;
    /// Last modification date and time.
    const QDateTime m_dateTime;
//    /// CRC.
//    quint32 crc;
//    /// Compressed file size.
//    quint32 compressedSize;
    /// Uncompressed file size.
    const qint64 m_uncompressedSize;
//    /// Disk number start.
//    quint16 diskNumberStart;
//    /// Internal file attributes.
//    quint16 internalAttr;
//    /// External file attributes.
//    quint32 externalAttr;
//    /// Comment.
//    QString comment;
//    /// Extra field.
//    QByteArray extra;
    explicit ArchiveFileInfo(const QString name,
                             const QDateTime dateTime,
                             const qint64 uncompressedSize)
        : m_name(name)
        , m_dateTime(dateTime)
        , m_uncompressedSize(uncompressedSize)
    {
    }
    bool operator==(const ArchiveFileInfo& rhs) const {
        return (m_name == rhs.m_name &&
                m_dateTime == rhs.m_dateTime &&
                m_uncompressedSize == rhs.m_uncompressedSize);
    }

    bool isDir() const { return m_uncompressedSize == 0; }
};

#endif  // ENUMS_H
