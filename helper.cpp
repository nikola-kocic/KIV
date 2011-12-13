#include "helper.h"
#include "settings.h"

//#include <QDebug>
#include <QtGui/QStyle>

bool checkFileExtension(const QFileInfo &info)
{
    if (isArchive(info) || info.isDir() || isImage(info))
    {
       return true;
    }
    else
    {
        return false;
    }
}

bool isArchive(const QFileInfo &fi)
{
    return Settings::Instance()->getFiltersArchive().contains(fi.suffix().toLower());
}

bool isImage(const QFileInfo &fi)
{
    return Settings::Instance()->getFiltersImage().contains(fi.suffix().toLower());
}


QIcon getDirectoryIcon()
{
    return QApplication::style()->standardIcon(QStyle::SP_DirIcon);
}

QIcon getFileIcon()
{
    return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
}
