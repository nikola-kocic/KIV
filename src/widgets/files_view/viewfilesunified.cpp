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

void ViewFilesUnified::pageNext()
{

}

void ViewFilesUnified::pagePrevious()
{

}

void ViewFilesUnified::setLocationUrl(const QUrl &url)
{
    const QString path = url.toLocalFile();
    const QModelIndex sourceIndex2 = mModelFilesystem->index(path);
    const QModelIndex current = mNestedModel->mapFromSource(sourceIndex2);
    setCurrentIndex(current);
}

void ViewFilesUnified::dirUp()
{

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
