#include "view_files.h"
#include <QHeaderView>

ViewFiles::TreeViewFiles::TreeViewFiles(ViewFiles *parent)
    : QTreeView(parent)
{
    m_parent = parent;


    this->setUniformRowHeights(true);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setRootIsDecorated(false);
    this->header()->setResizeMode(QHeaderView::ResizeToContents);
}
