#include "helper.h"
#include "settings.h"

//#include <QDebug>
#include <QtGui/QStyle>
#include <QImageReader>

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
    return getFiltersArchive().contains(fi.suffix().toLower());
}

bool isImage(const QFileInfo &fi)
{
    return getFiltersImage().contains(fi.suffix().toLower());
}


QIcon getDirectoryIcon()
{
    return QApplication::style()->standardIcon(QStyle::SP_DirIcon);
}

QIcon getFileIcon()
{
    return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
}

QStringList getFiltersArchive()
{
    QStringList filters_archive;

    filters_archive << "zip" << "cbz";

    return filters_archive;
}

QStringList getFiltersImage()
{
    QStringList filters_image;
    for (int i = 0; i < QImageReader::supportedImageFormats().size(); ++i)
    {
        filters_image.append(QImageReader::supportedImageFormats().at(i));
    }

    return filters_image;
}
