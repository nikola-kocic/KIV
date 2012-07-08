#ifndef ARCHIVE_MODEL_H
#define ARCHIVE_MODEL_H

#include "fileinfo.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QIcon>

class ArchiveItem;

struct ArchiveType
{
    static const int None = 0;
    static const int Zip = 1;
    static const int Rar = 2;
};

class ArchiveModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit ArchiveModel(const QString &path, QObject *parent = 0);
    ~ArchiveModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QModelIndex getDirectory(const QString &path); // Use with info.getZipPath()
    QModelIndex findIndexChild(const QString &text, const QModelIndex &root = QModelIndex());
    int getType() const;


private:
    ArchiveItem *getItem(const QModelIndex &index) const;
    ArchiveItem* AddNode(const QString &name, const QDateTime &date, const quint64 &bytes, const QString &path, const int type, ArchiveItem *parent = 0);
    QDateTime dateFromDos(const uint dosTime);

    ArchiveItem *rootItem;

    const QIcon m_icon_dir;
    QIcon m_icon_file;
    int m_type;
};

inline int ArchiveModel::getType() const
{ return m_type; }

#endif // ARCHIVE_MODEL_H
