#ifndef VIEWFILESUNIFIED_H
#define VIEWFILESUNIFIED_H

#include "archiveextractor.h"
#include "models/archivemodelhandler.h"

#include <QTreeView>
#include <QFileSystemModel>
#include <QUrl>

class ViewFilesUnified : public QTreeView
{
    Q_OBJECT
public:
    ViewFilesUnified(const IArchiveExtractor * const archive_extractor,
                     CustomFileSystemModel *modelFilesystem,
                     QWidget *parent = nullptr);
    void pageNext();
    void pagePrevious();
    void setLocationUrl(const QUrl &url);
    void dirUp();
    void setViewMode(const FileViewMode mode);
    FileInfo getCurrentFileInfo() const;
    void saveCurrentFile(const QString &fileName) const;
    void setThumbnailsSize(const QSize &size);
    void setShowThumbnails(const bool b);

protected:
    CustomFileSystemModel *mModelFilesystem;
    const IArchiveExtractor * const mArchiveExtractor;
    NestedModel<QString> *mNestedModel;
    FileInfo m_fileinfo_current;

    void on_filesystemView_currentRowChanged(const QModelIndex &current,
                                             const QModelIndex &previous);

    bool changeImage(std::function<int ()> generator, const QModelIndex& parent);
    NodeType getNodeType(const QModelIndex& index);
signals:
    void urlChanged(const QUrl &url);
};

#endif // VIEWFILESUNIFIED_H
