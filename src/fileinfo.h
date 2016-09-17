#ifndef FILEINFO_H
#define FILEINFO_H

#include <QFileInfo>
#include <QString>
#include <QStringList>

class FileInfo
{
public:
    explicit FileInfo(const QString &path, const bool isContainer = true);

    bool isValid() const;
    bool isInArchive() const;
    bool fileExists() const;
    bool isContainerValid() const;
    bool isContainerRoot() const;

     /* C:/Folder1/Folder2/image.png"
      * or C:/Folder1/Folder2/archive.zip/ZipFolder1/ZipFolder2/image.png" */
    QString getPath() const;

    QString getImageFileName() const; /* image.png */
    QString getContainerPath() const; /* C:/Folder1/Folder2" */

    /* "ZipFolder1/ZipFolder2/image.png" or "image.png" */
    QString getArchiveImagePath() const;

    /* "ZipFolder1/ZipFolder2/" (ends with '/') */
    QString getArchiveContainerPath() const;

    QString getContainerName() const;

    QString getDebugInfo() const;

private:
    QFileInfo m_container;
    QFileInfo m_image;

    QString m_zipPath;
    QString m_zipImageFileName;

    bool m_hasValidContainer;
    bool m_fileExists;
    bool m_isInArchive;
};

#endif  // FILEINFO_H
