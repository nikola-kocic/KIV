#ifndef MODELWRAPPER_H
#define MODELWRAPPER_H

#include <QObject>
#include <QSortFilterProxyModel>

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

class ModelWrapper : public QObject
{
    Q_OBJECT
public:
    explicit ModelWrapper(QObject *parent = 0);
    ~ModelWrapper();

signals:

public slots:
};

#endif // MODELWRAPPER_H
