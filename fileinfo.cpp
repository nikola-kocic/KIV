#include "fileinfo.h"
#include "helper.h"
#include <QFileInfo>
#include <QImageReader>

//#define DEBUG_FILE_INFO
#ifdef DEBUG_FILE_INFO
#include <QDebug>
#include <QDateTime>
#endif

QStringList FileInfo::m_filters_archive = QStringList() << "zip" << "cbz";
QStringList FileInfo::m_filters_image = QStringList();

FileInfo::FileInfo() :
    zipPath(""),
    zipImageFileName("")
{
    container = QFileInfo();
    image = QFileInfo();
}

QStringList FileInfo::getFiltersArchive()
{
    return m_filters_archive;
}

QStringList FileInfo::getFiltersImage()
{
    if (m_filters_image.isEmpty())
    {
        for (int i = 0; i < QImageReader::supportedImageFormats().size(); ++i)
        {
            m_filters_image.append(QImageReader::supportedImageFormats().at(i));
        }
    }
    return m_filters_image;
}

bool FileInfo::isArchive() const
{
    return !container.isDir();
}

bool FileInfo::fileExists() const
{
    bool exists = false;
    if (isValidContainer())
    {
        if (isArchive())
        {
            exists = !zipImageFileName.isEmpty();
        }
        else
        {
            exists = image.exists();
        }
    }
#ifdef DEBUG_FILE_INFO
    qDebug() << QDateTime::currentDateTime() << "FileInfo::fileExists()" << getFilePath() << "exists" << exists;
#endif

    return exists;
}

bool FileInfo::isValidContainer() const
{
    bool valid = container.exists();
#ifdef DEBUG_FILE_INFO
            qDebug() << QDateTime::currentDateTime() << "FileInfo::isValidContainer()" << container.canonicalFilePath() << "valid" << valid;
#endif
    return valid;
}

QString FileInfo::getFilePath() const
{
    if (isArchive())
    {
        return container.canonicalFilePath() + "/" + zipImagePath();
    }
    else if (!image.exists())
    {
        return container.canonicalFilePath();
    }
    else
    {
        return image.canonicalFilePath();
    }
}

QString FileInfo::zipImagePath() const
{
    return (zipPath.compare("/") == 0 ? "" : zipPath) + zipImageFileName;
}

QString FileInfo::getImageFileName() const
{
    if (isArchive())
    {
        return zipImageFileName;
    }
    else
    {
        return image.fileName();
    }
}

QString FileInfo::getDebugInfo() const
{
    QString str = "***\ncontainer: " + container.canonicalFilePath() + "\nimage: " + image.canonicalFilePath() + "\nzipImageFileName: " + zipImageFileName + "\nzipPath: " + zipPath + "\n***";
    return str;
}

FileInfo FileInfo::fromPath(const QString &path)
{
    FileInfo info;

    if (path.isEmpty())
        return info;


    QFileInfo fi(path);

    // If path is only directory or archive
    if (fi.isDir())
    {
#ifdef DEBUG_FILE_INFO
    qDebug() << QDateTime::currentDateTime() << "FileInfo::fromPath" << "fi.isDir()";
#endif
        info.container = fi.canonicalFilePath();
    }
    else if (isArchiveFile(fi))
    {
#ifdef DEBUG_FILE_INFO
    qDebug() << QDateTime::currentDateTime() << "FileInfo::fromPath" << "isArchive(fi)";
#endif
        info.container = fi.canonicalFilePath();
        info.zipPath = "/";
    }
    else
    {
#ifdef DEBUG_FILE_INFO
    qDebug() << QDateTime::currentDateTime() << "FileInfo::fromPath" << "not dir nor archive";
#endif
        // If path is image
        if (fi.exists())
        {
#ifdef DEBUG_FILE_INFO
    qDebug() << QDateTime::currentDateTime() << "FileInfo::fromPath" << "fi.exists()";
#endif
            info.container = fi.canonicalPath();
            info.image.setFile(fi.canonicalFilePath());
        }
        // If path is image in archive
        else
        {
#ifdef DEBUG_FILE_INFO
    qDebug() << QDateTime::currentDateTime() << "FileInfo::fromPath" << "!fi.exists()";
#endif
            QString temppath = path;
            int indexOfSlash = temppath.lastIndexOf('/');
#ifdef DEBUG_FILE_INFO
    qDebug() << QDateTime::currentDateTime() << "FileInfo::fromPath" << path << indexOfSlash;
#endif
            while (indexOfSlash != -1)
            {
#ifdef DEBUG_FILE_INFO
                qDebug() << QDateTime::currentDateTime() << "FileInfo::fromPath" << "index of /" << indexOfSlash << "path" << temppath;
#endif
                temppath = temppath.left(indexOfSlash);
                if (QFile::exists(temppath) && !QFileInfo(temppath).isDir())
                {
                    info.container = temppath;

                    QString zipAbsoluteFilePath = path.right(path.size() - temppath.size());

                    indexOfSlash = zipAbsoluteFilePath.lastIndexOf('/');

                    if (indexOfSlash != -1)
                    {
                        info.zipPath = zipAbsoluteFilePath.left(indexOfSlash + 1);
                        info.zipImageFileName = zipAbsoluteFilePath.right(zipAbsoluteFilePath.size() - indexOfSlash - 1);
                    }
                    else
                    {
                        info.zipPath = "/";
                    }

                    break;
                }
                indexOfSlash = temppath.lastIndexOf('/');
            }
        }
    }

#ifdef DEBUG_FILE_INFO
    qDebug() << QDateTime::currentDateTime() << "FileInfo::fromPath" << "path:" << path << "containerPath" << info.container.canonicalFilePath() << "imageFileName" << info.image.canonicalFilePath();
    qDebug() << QDateTime::currentDateTime() << "zipPath" << info.zipPath << "zipImagePath" <<info.zipImageFileName;
#endif

    return info;
}
