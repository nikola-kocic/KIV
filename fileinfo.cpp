#include "fileinfo.h"
#include "helper.h"
#include <QFileInfo>

//#define DEBUG_FILE_INFO
#ifdef DEBUG_FILE_INFO
#include <QDebug>
#include <QDateTime>
#endif

FileInfo::FileInfo() :
    containerPath(""),
    imageFileName(""),
    zipPath(""),
    zipImageFileName(""),
    thumbSize(0)
{
}

bool FileInfo::isZip() const
{
    return !zipPath.isEmpty();
}

bool FileInfo::fileExists() const
{
    bool exists = false;
    if (isValidContainer())
    {
        if (isZip())
        {
            exists = !zipImageFileName.isEmpty();
        }
        else if (!imageFileName.isEmpty())
        {
            exists = QFile::exists(getFilePath());
        }
    }
#ifdef DEBUG_FILE_INFO
    qDebug() << QDateTime::currentDateTime() << "FileInfo::fileExists()" << getFilePath() << "exists" << exists;
#endif

    return exists;
}

bool FileInfo::isValidContainer() const
{
    bool valid = false;
    if (!containerPath.isEmpty())
    {
        valid = QFile::exists(containerPath);
    }
#ifdef DEBUG_FILE_INFO
            qDebug() << QDateTime::currentDateTime() << "FileInfo::isValidContainer()" << containerPath << "valid" << valid;
#endif

    return valid;
}

QString FileInfo::getFilePath() const
{
    if (isZip())
    {
        return containerPath + "/" + zipImagePath();
    }
    else
    {
        return containerPath + "/" + imageFileName;
    }
}

QString FileInfo::zipImagePath() const
{
    return (zipPath.compare("/") == 0 ? "" : zipPath) + zipImageFileName;
}

QString FileInfo::getImageFileName() const
{
    if (isZip())
    {
        return zipImageFileName;
    }
    else
    {
        return imageFileName;
    }
}

FileInfo FileInfo::fromPath(const QString &path)
{
    FileInfo info;

    if (path.isEmpty())
        return info;


    QFileInfo fi(path);

    // If path is only directory or archive
    if (fi.isDir() || isArchive(fi))
    {
        info.containerPath = fi.absoluteFilePath();
    }
    else
    {
        // If path is image
        if (fi.exists())
        {
            info.containerPath = fi.absolutePath();
            info.imageFileName = fi.fileName();
        }
        // If path is image in archive
        else
        {
            QString absoluteFilePath = fi.absoluteFilePath();
            QString temppath = absoluteFilePath;
            int indexOfSlash = temppath.lastIndexOf('/');
            while (indexOfSlash != -1)
            {
#ifdef DEBUG_FILE_INFO
                qDebug() << QDateTime::currentDateTime() << "FileInfo::fromPath" << "index of /" << indexOfSlash << "path" << temppath;
#endif
                temppath = temppath.left(indexOfSlash);
                if (QFile::exists(temppath) && !QFileInfo(temppath).isDir())
                {
                    info.containerPath = temppath;

                    QString zipAbsoluteFilePath = absoluteFilePath.right(absoluteFilePath.size() - temppath.size());

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
    qDebug() << QDateTime::currentDateTime() << "FileInfo::fromPath" << "path:" << path << "containerPath" << info.containerPath << "imageFileName" << info.imageFileName;
    qDebug() << QDateTime::currentDateTime() << "zipPath" << info.zipPath << "zipImagePath" <<info.zipImageFileName;
#endif

    return info;
}
