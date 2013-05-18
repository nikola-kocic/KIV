#include "fileinfo.h"

#include <QDir>
#include <QFileInfo>

//#define DEBUG_FILE_INFO
#ifdef DEBUG_FILE_INFO
#include <QDebug>
#include <QDateTime>
#endif

#include "helper.h"

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
    Helper::debuglog(Q_FUNC_INFO, "called with path: " + path);
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
        qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "FileInfo::FileInfo" << "path is path to directory\n" << getDebugInfo() << "\n\n";
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
        qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "FileInfo::FileInfo"
                 << "path is path to zip file\n" << getDebugInfo() << "\n\n";
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
        qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "FileInfo::FileInfo"
                 << "path is image\n" << getDebugInfo() << "\n\n";
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
//        qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "FileInfo::FileInfo" << "index of /" << indexOfContainerSlash << "path" << temppath;
//#endif
        tempContainerPath.truncate(indexOfContainerSlash);
        const QFileInfo tempFileInfo = QFileInfo(tempContainerPath);
        if (tempFileInfo.exists() && !tempFileInfo.isDir())
        {
            m_container = tempFileInfo;
            m_hasValidContainer = true;
            m_isInArchive = true;

            const QString zipAbsoluteFilePath = editedPath.right(editedPath.size() - tempContainerPath.size() - 1);

            if (!zipAbsoluteFilePath.isEmpty())
            {
                int indexOfZipSlash = zipAbsoluteFilePath.lastIndexOf('/');

                if (indexOfZipSlash != -1)
                {
                    m_zipPath = zipAbsoluteFilePath.left(indexOfZipSlash + 1);
                }

                if (indexOfZipSlash != zipAbsoluteFilePath.size() - 1)
                {
                    m_zipImageFileName = zipAbsoluteFilePath.right(zipAbsoluteFilePath.size() - indexOfZipSlash - 1);
                    m_fileExists = true;
                }
            }

#ifdef DEBUG_FILE_INFO
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "FileInfo::FileInfo" << "path is image in archive\n" << getDebugInfo() << "\n\n";
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
        path = m_container.canonicalFilePath() + "/" + zipImagePath();
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

QString FileInfo::zipImagePath() const
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

QString FileInfo::getZipPath() const
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
    const QString str = "***\nContainer: " + getContainerPath() + "\nPath: " + getPath()
            + "\nImageFileName: " + getImageFileName()
            + "\nZipPath: " + getZipPath()
            + "\nIsInArchive: " + (isInArchive() ? "true" : "false")
            + "\nContainer Name: " + getContainerName()
            + "\n***";
    return str;
}
