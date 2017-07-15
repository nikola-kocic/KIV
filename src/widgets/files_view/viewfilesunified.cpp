#include "include/optional.hpp"
#include "viewfilesunified.h"
#include "models/nestedmodel.h"
#include "models/archivemodelhandler.h"
#include <memory>
#include <QFileSystemModel>

using std::experimental::optional;

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
    mNodeNavigator = new NodeNavigator(mNestedModel, new NodeIdentifier());
    setModel(mNestedModel);
    setRootIndex(mNestedModel->mapFromSource(mModelFilesystem->index("/")));

    // Only show first column
    for (int i = 1; i < mNestedModel->columnCount(); ++i) {
        setColumnHidden(i, true);
    }

    connect(selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this, &ViewFilesUnified::on_filesystemView_currentRowChanged);
    connect(mNodeNavigator, &NodeNavigator::navigated, [this](QModelIndex index){
        if (index.isValid()) {
            setCurrentIndex(index);
        }
    });
    connect(mNestedModel, &QAbstractItemModel::rowsInserted, [this](
            const QModelIndex &parent, int /*first*/, int /*last*/) {
        /*
         * In both this and QFileSystemModel::directoryLoaded slot,
         * child nodes are not yet sorted.
         * They will be sorted in QAbstractItemModel::layoutChanged.
         * Wait with navigation until then.
        */
        mLastDirLoaded = parent;
        // But if there are no child nodes, continue navigation
        if (mNestedModel->rowCount(parent) == 0) {
            mNodeNavigator->nodeLoaded(parent);
            mLastDirLoaded = QPersistentModelIndex();
        }
    });
    connect(mNestedModel, &QAbstractItemModel::layoutChanged, [this](
            const QList<QPersistentModelIndex> &/*parents*/,
            QAbstractItemModel::LayoutChangeHint /*hint*/) {
        // Child nodes should be sorted here, so continue navigation
        mNodeNavigator->nodeLoaded(mLastDirLoaded);
        mLastDirLoaded = QPersistentModelIndex();
    });
}

bool ViewFilesUnified::setLocationUrl(const QUrl &url)
{
    const QString path = url.toLocalFile();
    const FileInfo fileinfo(path);
    const QModelIndex index = mNestedModel->indexFromIdentifiers(fileinfo.getIdentifiers());
    if (index.isValid()) {
        setCurrentIndex(index);
        return true;
    }
    return false;
}

void ViewFilesUnified::dirUp()
{
    const QModelIndex parent = currentIndex().parent();
    if (parent.isValid()) {
        setCurrentIndex(parent);
    }
}

void ViewFilesUnified::setViewMode(const FileViewMode /*mode*/)
{

}

FileInfo ViewFilesUnified::getCurrentFileInfo() const
{
    return m_fileinfo_current;
}

void ViewFilesUnified::saveCurrentFile(const QString& /*fileName*/) const
{

}

void ViewFilesUnified::setThumbnailsSize(const QSize& /*size*/)
{

}

void ViewFilesUnified::setShowThumbnails(const bool /*b*/)
{

}

void ViewFilesUnified::on_filesystemView_currentRowChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
    const Identifiers<QString> identifiers = mNestedModel->identifiersFromIndex(current);
    const QString filePath = QDir::cleanPath(identifiers.parentIdentifier + '/' + identifiers.childIdentifier);
    // Pass 'false' for 'IsContainer' because item is only selected
    m_fileinfo_current = FileInfo(filePath, false);
    emit urlChanged(QUrl::fromLocalFile(filePath));
}

void ViewFilesUnified::imagePrevious()
{
    mNodeNavigator->getPreviousImage(currentIndex());
}

void ViewFilesUnified::imageNext()
{
    mNodeNavigator->getNextImage(currentIndex());
}
