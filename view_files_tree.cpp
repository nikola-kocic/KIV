#include "view_files.h"
#include <QHeaderView>

TreeViewFiles::TreeViewFiles(QWidget *parent)
    : QTreeView(parent)
{
    this->setUniformRowHeights(true);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setRootIsDecorated(false);
    this->header()->setResizeMode(QHeaderView::ResizeToContents);
}
