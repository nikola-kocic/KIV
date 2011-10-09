#include "system_icons.h"

#include <QtGui/QApplication>
#include <QtGui/QStyle>

QIcon SystemIcons::getDirectoryIcon()
{
    return QApplication::style()->standardIcon(QStyle::SP_DirIcon);
}
