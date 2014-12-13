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

struct SortDirection
{
    static const int NameAsc = 0;
    static const int NameDesc = 1;
    static const int DateAsc = 2;
    static const int DateDesc = 3;
    static const int SizeAsc = 4;
    static const int SizeDesc = 5;
};

struct ArchiveFileInfo {
    /// File name.
    QString name;
//    /// Version created by.
//    quint16 versionCreated;
//    /// Version needed to extract.
//    quint16 versionNeeded;
//    /// General purpose flags.
//    quint16 flags;
//    /// Compression method.
//    quint16 method;
    /// Last modification date and time.
    QDateTime dateTime;
//    /// CRC.
//    quint32 crc;
//    /// Compressed file size.
//    quint32 compressedSize;
    /// Uncompressed file size.
    quint32 uncompressedSize;
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
    explicit ArchiveFileInfo(QString name,
                             QDateTime dateTime,
                             quint32 uncompressedSize)
        :name(name)
        , dateTime(dateTime)
        , uncompressedSize(uncompressedSize)
    {
    }
    bool operator==(const ArchiveFileInfo& other) const {
        return (name == other.name &&
                dateTime == other.dateTime &&
                uncompressedSize == other.uncompressedSize);
    }

    bool isDir() const { return uncompressedSize == 0; }
};

#endif  // ENUMS_H
