#ifndef FILEINFO_H
#define FILEINFO_H

#include <QFileInfo>
#include <QString>
#include <QStringList>

template <class TIdentifier>
struct Identifiers {
    Identifiers(const TIdentifier& parentIdentifier, const TIdentifier& childIdentifier)
        : parentIdentifier(parentIdentifier)
        , childIdentifier(childIdentifier)
    {
    }
    TIdentifier parentIdentifier;
    TIdentifier childIdentifier;
};


class FileInfo
{
public:
    explicit FileInfo(const QString &path, const bool isContainer = true);

    bool isValid() const;
    bool fileExists() const;

    Identifiers<QString> getIdentifiers() const;

     /* C:/Folder1/Folder2/image.png"
      * or C:/Folder1/Folder2/archive.zip/ZipFolder1/ZipFolder2/image.png" */
    QString getPath() const;

    QString getDebugInfo() const;

private:
    QFileInfo m_container;
    QFileInfo m_image;

    QString m_zipPath;
    QString m_zipImageFileName;

    bool m_hasValidContainer;
    bool m_fileExists;
    bool m_isInArchive;

private:
    bool isInArchive() const;
    bool isContainerValid() const;
    /* "ZipFolder1/ZipFolder2/image.png" or "image.png" */
    QString getArchiveImagePath() const;
    QString getContainerPath() const; /* C:/Folder1/Folder2" */


};

#endif  // FILEINFO_H
