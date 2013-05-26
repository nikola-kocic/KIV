#include "fileinfo.h"

#include <QDir>
#include <QFileInfo>

#include "helper.h"

//#define DEBUG_FILE_INFO
#ifdef DEBUG_FILE_INFO
#include "helper.h"
#endif

FileInfo::FileInfo(const QString &path, const bool isContainer)
    : m_container(QFileInfo())
    , m_hasValidContainer(false)
    , m_image(QFileInfo())
    , m_fileExists(false)
    , m_zipPath("")
    , m_zipImageFileName("")
    , m_isInArchive(false)
{
#ifdef DEBUG_FILE_INFO
    DEBUGOUT << "called with path: " << path;
#endif
    if (path.isEmpty())
    {
        return;
    }

    const QFileInfo fi(path);

    if (fi.isDir())
    {
        // If path is path to directory
        m_container = fi;
        m_hasValidContainer = true;

#ifdef DEBUG_FILE_INFO
        DEBUGOUT << "path is path to directory" << getDebugInfo();
#endif
        return;
    }


    if (Helper::isArchiveFile(fi))
    {
        // If path is path to zip file
        m_container = fi;
        m_hasValidContainer = true;

        if (isContainer)
        {
            m_isInArchive = true;
        }
#ifdef DEBUG_FILE_INFO
        DEBUGOUT << "path is path to zip file" << getDebugInfo();
#endif
        return;
    }


    if (fi.exists())
    {
        if (!Helper::isImageFile(fi))
            return;

        // If path is image
        m_container = QFileInfo(fi.canonicalPath());
        m_hasValidContainer = true;

        m_image = fi;
        m_fileExists = true;

#ifdef DEBUG_FILE_INFO
        DEBUGOUT << "path is image\n\t" << getDebugInfo();
#endif
        return;
    }


    // If path is image in archive

    const QString editedPath = QDir::fromNativeSeparators(path);
    QString tempContainerPath = editedPath;
    int indexOfContainerSlash = -1;
    do
    {
        indexOfContainerSlash = tempContainerPath.lastIndexOf('/');

//#ifdef DEBUG_FILE_INFO
//        DEBUGOUT << "index of /" << indexOfContainerSlash << "path" << temppath;
//#endif
        tempContainerPath.truncate(indexOfContainerSlash);
        const QFileInfo tempFileInfo = QFileInfo(tempContainerPath);
        if (tempFileInfo.exists() && !tempFileInfo.isDir())
        {
            m_container = tempFileInfo;
            m_hasValidContainer = true;
            m_isInArchive = true;

            const QString zipAbsoluteFilePath =
                    editedPath.right(editedPath.size()
                                     - tempContainerPath.size() - 1);

            if (!zipAbsoluteFilePath.isEmpty())
            {
                int indexOfZipSlash = zipAbsoluteFilePath.lastIndexOf('/');

                if (indexOfZipSlash != -1)
                {
                    m_zipPath = zipAbsoluteFilePath.left(indexOfZipSlash + 1);
                }

                if (indexOfZipSlash != zipAbsoluteFilePath.size() - 1)
                {
                    m_zipImageFileName = zipAbsoluteFilePath.right(
                                zipAbsoluteFilePath.size()
                                - indexOfZipSlash - 1);
                    m_fileExists = true;
                }
            }

#ifdef DEBUG_FILE_INFO
            DEBUGOUT << "path is image in archive\n\t" << getDebugInfo();
#endif
            return;
        }
    }
    while (indexOfContainerSlash != -1);
}

bool FileInfo::isValid() const
{
    return (this->fileExists() || this->isContainerValid());
}

bool FileInfo::isInArchive() const
{
    return m_isInArchive;
}

bool FileInfo::fileExists() const
{
    return m_fileExists;
}

bool FileInfo::isContainerValid() const
{
    return m_hasValidContainer;
}

QString FileInfo::getPath() const
{
    QString path;
    if (isInArchive())
    {
        path = m_container.canonicalFilePath() + "/" + getArchiveImagePath();
    }
    else if (fileExists())
    {
        path = m_image.canonicalFilePath();
    }
    else
    {
        path = m_container.canonicalFilePath();
    }
    return path;
}

QString FileInfo::getArchiveImagePath() const
{
    return m_zipPath + m_zipImageFileName;
}

QString FileInfo::getImageFileName() const
{
    if (isInArchive())
    {
        return m_zipImageFileName;
    }
    else
    {
        return m_image.fileName();
    }
}

QString FileInfo::getContainerPath() const
{
    return m_container.canonicalFilePath();
}

QString FileInfo::getArchiveContainerPath() const
{
    return m_zipPath;
}

QString FileInfo::getContainerName() const
{
    const QString containerName = m_container.fileName();
    return containerName;
}

bool FileInfo::isContainerRoot() const
{
    return m_container.isRoot();
}

QString FileInfo::getDebugInfo() const
{
    const QString str = "\n\tContainer: " + getContainerPath()
            + "\n\tPath: " + getPath()
            + "\n\tImageFileName: " + getImageFileName()
            + "\n\tZipPath: " + getArchiveContainerPath()
            + "\n\tIsInArchive: " + (isInArchive() ? "true" : "false")
            + "\n\tContainer Name: " + getContainerName();
    return str;
}
