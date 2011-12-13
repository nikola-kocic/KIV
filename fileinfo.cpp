#include "fileinfo.h"
#include <QDebug>
FileInfo::FileInfo() :
    containerPath(""),
    imageFileName(""),
    zipPathToImage(""),
    zipImageFileName(""),
    thumbSize(0)
{
}

bool FileInfo::isZip() const
{
//    qDebug() << zipImageFileName;
    return !zipPathToImage.isEmpty();
}

QString FileInfo::getFilePath() const
{
    if (isZip())
    {
        return containerPath + "/" + zipPathToImage + zipImageFileName;
    }
    else
    {
        return containerPath + "/" + imageFileName;
    }
}

QString FileInfo::zipImagePath() const
{
    return (zipPathToImage.compare("/") == 0 ? "" : zipPathToImage) + zipImageFileName;
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
