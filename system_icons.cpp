#include "system_icons.h"

#include <QtGui/QStyle>

QIcon SystemIcons::getDirectoryIcon()
{
    return QApplication::style()->standardIcon(QStyle::SP_DirIcon);
}

QIcon SystemIcons::getFileIcon()
{
    return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
}
