#include "widgets/files_view/fileviewwithproxy.h"


FileViewWithProxy::FileViewWithProxy(
        std::unique_ptr<QAbstractItemView> view,
        std::unique_ptr<QAbstractProxyModel> proxy,
        QWidget *parent)
    : IFileView(parent)
    , m_view(std::move(view))
    , m_proxy(std::move(proxy))
{
    m_view->setModel(m_proxy.get());
    connect(m_view->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &FileViewWithProxy::currentChanged);
}

QWidget *FileViewWithProxy::getWidget()
{
    return m_view.get();
}

void FileViewWithProxy::setModel(QAbstractItemModel *model)
{
    m_proxy->setSourceModel(model);
}

QModelIndex FileViewWithProxy::currentIndex() const
{
    return m_proxy->mapToSource(m_view->currentIndex());
}

void FileViewWithProxy::currentChanged(
        const QModelIndex &current,
        const QModelIndex &/*previous*/)
{
    emit currentRowChanged(m_proxy->mapToSource(current));
}

void FileViewWithProxy::setCurrentIndex(const QModelIndex &index)
{
    m_view->setCurrentIndex(m_proxy->mapFromSource(index));
}
