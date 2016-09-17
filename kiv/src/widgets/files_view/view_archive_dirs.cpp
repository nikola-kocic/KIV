#include "view_archive_dirs.h"

#include <QHeaderView>

#include "kiv/src/helper.h"
#include "kiv/src/models/archive_model.h"




ViewArchiveDirs::ViewArchiveDirs(
        std::unique_ptr<QTreeView> view,
        std::unique_ptr<QAbstractProxyModel> proxy,
        QWidget *parent)
    : FileViewWithProxy(std::move(view), std::move(proxy), parent)
{
    QTreeView *const t = static_cast<QTreeView*>(m_view.get());
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setUniformRowHeights(true);
    t->header()->setSectionResizeMode(QHeaderView::Stretch);
    t->setHeaderHidden(true);
}

void ViewArchiveDirs::setModel(QAbstractItemModel *model)
{
    FileViewWithProxy::setModel(model);

    QTreeView *const t = static_cast<QTreeView*>(m_view.get());
    for (int i = 1; i < t->header()->count(); ++i)
    {
        t->hideColumn(i);
    }
}

void ViewArchiveDirs::setCurrentIndex(const QModelIndex &index)
{
    FileViewWithProxy::setCurrentIndex(index);

    QModelIndex expandIndex = m_view->currentIndex();
    QTreeView *const t = static_cast<QTreeView*>(m_view.get());
    while (!t->isExpanded(expandIndex) && expandIndex.isValid())
    {
        t->expand(expandIndex);
        expandIndex = expandIndex.parent();
    }
}
