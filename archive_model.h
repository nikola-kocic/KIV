#ifndef ARCHIVE_MODEL_H
#define ARCHIVE_MODEL_H

#include "fileinfo.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QIcon>

class ArchiveItem;

class ArchiveModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit ArchiveModel(QObject *parent = 0);
    ~ArchiveModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void setPath(const QString &path);
    QModelIndex getDirectory(const QString &path);
    QModelIndex findIndexChild(const QString &text, const QModelIndex &root = QModelIndex());
    void clear();

private:
    ArchiveItem *getItem(const QModelIndex &index) const;
    ArchiveItem* AddNode(const QString &name, const QDateTime &date, const quint64 &bytes, const QString &path, int type, ArchiveItem *parent = 0);

    ArchiveItem *rootItem;
    ArchiveItem *rootArchiveItem;

    QIcon m_icon_dir;
    QIcon m_icon_file;
};

#endif // ARCHIVE_MODEL_H
