#include "viewfilesunified.h"
#include "models/nestedmodel.h"
#include "models/archivemodelhandler.h"
#include <memory>

ViewFilesUnified::ViewFilesUnified(
        const IArchiveExtractor * const archive_extractor, CustomFileSystemModel *model_filesystem, QWidget *parent)
    : QTreeView (parent)
    , mModelFilesystem(model_filesystem)
    , mArchiveExtractor(archive_extractor)
    , m_fileinfo_current(FileInfo(""))
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setUniformRowHeights(true);
    setHeaderHidden(true);

    mModelFilesystem->setRootPath("");

    std::unique_ptr<ArchiveModelHandler> archiveModelHandler =
            std::make_unique<ArchiveModelHandler>(mModelFilesystem, archive_extractor);
    mNestedModel = new NestedModel<QString>(std::move(archiveModelHandler));
    setModel(mNestedModel);
    setRootIndex(
                mNestedModel->mapFromSource(
                    mModelFilesystem->index("/")
                    )
                );

    // Only show first column
    for (int i = 1; i < mNestedModel->columnCount(); ++i) {
        setColumnHidden(i, true);
    }

    connect(selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this, &ViewFilesUnified::on_filesystemView_currentRowChanged);
}

void ViewFilesUnified::setLocationUrl(const QUrl &url)
{
    const QString path = url.toLocalFile();
    const FileInfo fileinfo(path);
    // TODO: Move this logic, for example "NestedModel::identifiersFromIndex" and "FileInfo::getIdentifiers"
    if (fileinfo.isInArchive()) {
        const QString containerPath = fileinfo.getContainerPath();
        const QString pathInArchive = fileinfo.getArchiveImagePath();
        const QModelIndex index = mNestedModel->indexFromIdentifiers(containerPath, pathInArchive);
        setCurrentIndex(index);
    } else {
        const QModelIndex current = mNestedModel->indexFromIdentifiers(path, "");
        setCurrentIndex(current);
    }
}

void ViewFilesUnified::dirUp()
{
    setCurrentIndex(currentIndex().parent());
}

void ViewFilesUnified::setViewMode(const FileViewMode mode)
{

}

FileInfo ViewFilesUnified::getCurrentFileInfo() const
{
    return m_fileinfo_current;
}

void ViewFilesUnified::saveCurrentFile(const QString &fileName) const
{

}

void ViewFilesUnified::setThumbnailsSize(const QSize &size)
{

}

void ViewFilesUnified::setShowThumbnails(const bool b)
{

}

void ViewFilesUnified::on_filesystemView_currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    const QString filePath =
            current.data(QFileSystemModel::FilePathRole).toString();
    // Pass 'false' for 'IsContainer' because item is only selected
    m_fileinfo_current = FileInfo(filePath, false);
    emit urlChanged(QUrl::fromLocalFile(filePath));
}

bool ViewFilesUnified::changeImage(std::function<int()> generator, const QModelIndex& parent)
{
    bool imageChanged = false;
    int i = 0;
    while ((i = generator()) != -1)
    {
        const QModelIndex index = model()->index(i, 0, parent);
        const QModelIndex source_index = mNestedModel->mapToSource(index);
        if (getNodeType(source_index) == NodeType::Image)
        {
            setCurrentIndex(index);
            imageChanged = true;
            break;
        }
    }
    return imageChanged;
}

NodeType ViewFilesUnified::getNodeType(const QModelIndex &index)
{
    bool ok = false;
    const int nodeTypeInt = index.data(Helper::ROLE_NODE_TYPE).toInt(&ok);
    Q_ASSERT(ok);
    return NodeType(nodeTypeInt);
}

void ViewFilesUnified::pageNext()
{
    const QModelIndex currentProxyIndex = currentIndex();
    const QModelIndex parentProxyIndex = currentProxyIndex.parent();
    if (!parentProxyIndex.isValid()) {
        Q_ASSERT(false);
        return;
    }
    const int startRow = currentProxyIndex.isValid() ? (currentProxyIndex.row() + 1) : 0;
    const int totalRows = model()->rowCount(currentProxyIndex.parent());
    const std::function<int()> generator = [startRow, totalRows] {
        int i = startRow;
        return [=]() mutable { return i < totalRows ? i++ : -1; };
    }();
    const bool hasNext = changeImage(generator, parentProxyIndex);
}

void ViewFilesUnified::pagePrevious()
{
    const QModelIndex currentProxyIndex = currentIndex();
    const QModelIndex parentProxyIndex = currentProxyIndex.parent();
    if (!parentProxyIndex.isValid()) {
        Q_ASSERT(false);
        return;
    }
    const int totalRows = model()->rowCount(parentProxyIndex);
    const int startRow = currentProxyIndex.isValid() ? (currentProxyIndex.row() - 1) : (totalRows - 1);
    const std::function<int()> generator = [startRow] {
        int i = startRow;
        return [=]() mutable { return i >= 0 ? i-- : -1; };
    }();
    const bool hasPrevious = changeImage(generator, parentProxyIndex);
}
