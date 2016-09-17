#ifndef MODELWRAPPER_H
#define MODELWRAPPER_H

#include <QObject>
#include <QSortFilterProxyModel>

#include "enums.h"
#include "helper.h"
#include "models/archive_model.h"
#include "models/filesystem_model.h"



class IModelWrapper
{
public:
    virtual ~IModelWrapper() {}
    virtual NodeType getNodeType(const QModelIndex &index) const = 0;
    virtual QAbstractItemModel *getModel() = 0;
    virtual QModelIndex getContainer(const FileInfo &info) const  = 0;
    virtual QModelIndex getFile(const FileInfo &info, const QModelIndex& container) const = 0;
    virtual QString filePath(const QModelIndex &index) const = 0;
    virtual bool isDir(const QModelIndex &index) const = 0;
    virtual QFileInfo fileInfo(const QModelIndex &index) const = 0;
};


class ArchiveModelWrapper : public IModelWrapper
{
public:
    explicit ArchiveModelWrapper(ArchiveModel *model);
    NodeType getNodeType(const QModelIndex &index) const override;
    QAbstractItemModel *getModel() override;
    QModelIndex getContainer(const FileInfo &info) const override;
    QModelIndex getFile(const FileInfo &info, const QModelIndex& container) const override;
    QString filePath(const QModelIndex &index) const override;
    bool isDir(const QModelIndex &index) const override;
    QFileInfo fileInfo(const QModelIndex &index) const override;

protected:
    ArchiveModel *m_model;
};



class FileSystemModelWrapper : public IModelWrapper
{
public:
    explicit FileSystemModelWrapper(FileSystemModel *model);
    NodeType getNodeType(const QModelIndex &index) const override;
    QAbstractItemModel *getModel() override;
    QModelIndex getContainer(const FileInfo &info) const override;
    QModelIndex getFile(const FileInfo &info, const QModelIndex& /*container*/) const override;
    QString filePath(const QModelIndex &index) const override;
    bool isDir(const QModelIndex &index) const override;
    QFileInfo fileInfo(const QModelIndex &index) const override;

protected:
    FileSystemModel *m_model;
};



class FileListSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit FileListSortFilterProxyModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {}
protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

class ContainersSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit ContainersSortFilterProxyModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {}
protected:
    bool filterAcceptsColumn(int source_column,
                             const QModelIndex &source_parent) const override;
    bool filterAcceptsRow(int source_row,
                          const QModelIndex &source_parent) const override;
};

class ArchiveDirsSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit ArchiveDirsSortFilterProxyModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {}

protected:
    bool filterAcceptsColumn(int source_column,
                             const QModelIndex &source_parent) const override;
    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;
};


#endif // MODELWRAPPER_H
