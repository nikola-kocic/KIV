#ifndef ARCHIVEMODELHANDLER_H
#define ARCHIVEMODELHANDLER_H

#include "archive_model.h"
#include "nestedmodel.h"
#include "archiveextractor.h"
#include "helper.h"
#include "enums.h"

#include <QFileInfo>
#include <QModelIndex>
#include <QFileSystemModel>

class CustomFileSystemModel : public QFileSystemModel {
    Q_OBJECT
public:
    CustomFileSystemModel(QObject *parent = nullptr): QFileSystemModel (parent) { }
    QModelIndex createIndexMine(int arow, int acolumn, quintptr i) const {
        return createIndex(arow, acolumn, i);
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (role == Helper::ROLE_NODE_TYPE) {
            const QFileInfo indexFileInfo = fileInfo(index);
            if (Helper::isImageFileExtension(indexFileInfo))
            {
                return NodeType::Image;
            }
            else if (Helper::isArchiveFile(indexFileInfo))
            {
                return NodeType::Archive;
            }
            else
            {
                return NodeType::Directory;
            }
        }
        return QFileSystemModel::data(index, role);
    }
};


class ArchiveModelHandler : public NestedModelHandler<QString> {
protected:
    CustomFileSystemModel* mFileSystemModel;
    const IArchiveExtractor* mArchiveReader;

public:
    ArchiveModelHandler(CustomFileSystemModel* fileSystemModel, const IArchiveExtractor* archiveReader)
        : mFileSystemModel(fileSystemModel)
        , mArchiveReader(archiveReader) {
    }

    QAbstractItemModel* getParentModel() const override {
        return mFileSystemModel;
    }

    QString getParentIdentifierFromIndex(const QModelIndex &index) const override {
        return mFileSystemModel->filePath(index);
    }


    QModelIndex getParentIndexFromIdentifier(const QString& identifier) const override {
        return mFileSystemModel->index(identifier);
    }

    QAbstractItemModel* createChildModel(const QModelIndex& parentIndex) const override {
        const QString path = mFileSystemModel->filePath(parentIndex);
        std::vector<ArchiveFileInfo> list;
        mArchiveReader->getFileInfoList(path, list);

        QAbstractItemModel* arm = new ArchiveModel(list);
        return arm;
    }

    bool shouldHaveChilModel(const QModelIndex& parentIndex) const override {
        bool shouldHaveChilModel = false;
        const QFileInfo fi = mFileSystemModel->fileInfo(parentIndex);
        QStringList archiveExtensions = QStringList() << "zip" << "rar" << "7z";
        shouldHaveChilModel = archiveExtensions.contains(fi.suffix().toLower());
        return shouldHaveChilModel;
    }

    QModelIndex createChildIndex(const QAbstractItemModel* model, int arow, int acolumn, quintptr i) const override {
        return dynamic_cast<const ArchiveModel*>(model)->createIndexMine(arow, acolumn, i);
    }

    QModelIndex createParentIndex(const QAbstractItemModel* model, int arow, int acolumn, quintptr i) const override {
        return dynamic_cast<const CustomFileSystemModel*>(model)->createIndexMine(arow, acolumn, i);
    }

    int getColumnCount() const override {
        return 3;  // TODO: Don't hardcode
    }

    QModelIndex getChildIndexFromIdentifier(const QAbstractItemModel* childModel, const QString& identifier) const override {
        return dynamic_cast<const ArchiveModel*>(childModel)->getIndexFromPath(identifier);
    }

    QString getNullIdentifier() const override {
        return QString();
    }

    QString getChildIdentifierFromIndex(const QModelIndex &childIndex) const override {
        const ArchiveModel* archiveModel = dynamic_cast<const ArchiveModel*>(childIndex.model());
        Q_ASSERT(archiveModel != nullptr);
        return archiveModel->getIndexIdentifier(childIndex);
    }
};

#endif // ARCHIVEMODELHANDLER_H
