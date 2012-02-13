#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include <QStringList>
#include <QFileInfo>

class FileInfo
{
public:
    explicit FileInfo(const QString &path, const bool isContainer = true);

    bool isValid() const;
    bool isInArchive() const;
    bool fileExists() const;
    bool isContainerValid() const;

    QString getPath() const; /* C:/Folder1/Folder2/image.png" or C:/Folder1/Folder2/archive.zip/ZipFolder1/ZipFolder2/image.png" */
    QString zipImagePath() const;  /* "ZipFolder1/ZipFolder2/image.png" or "image.png" */
#ifdef WIN32
    QString rarImagePath() const;  /* "ZipFolder1\ZipFolder2\image.png" or "image.png" */
#endif
    QString getImageFileName() const; /* image.png */
    QString getContainerPath() const; /* C:/Folder1/Folder2" */
    QString getZipPath() const; /* "ZipFolder1/ZipFolder2/" (ends with '/') */
    QString getContainerName() const;
    bool isContainerRoot() const;


    QString getDebugInfo() const;

private:
    QFileInfo m_container;
    bool m_hasValidContainer;

    QFileInfo m_image;
    bool m_fileExists;

    QString m_zipPath;
    QString m_zipImageFileName;
    bool m_isInArchive;
};

#endif // FILEINFO_H
