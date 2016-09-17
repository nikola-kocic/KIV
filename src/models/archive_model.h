#ifndef ARCHIVE_MODEL_H
#define ARCHIVE_MODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QIcon>
#include <QModelIndex>
#include <QVariant>

#include "enums.h"
#include "models/archive_item.h"
#include "fileinfo.h"


class ArchiveModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit ArchiveModel(const std::vector<ArchiveFileInfo> &archive_files,
                          const QString &path,
                          QObject *const parent = nullptr);
    ~ArchiveModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row,
                      int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex getDirectory(const QString &path);  // Use with info.getZipPath()
    QModelIndex findIndexChild(const QString &text,
                               const QModelIndex &root = QModelIndex());
    ArchiveItem *getItem(const QModelIndex &index) const;


private:
    ArchiveItem* AddNode(const QString &name,
                         const QDateTime &date,
                         const qint64 &bytes,
                         const QString &path,
                         const NodeType type,
                         ArchiveItem *const parent = nullptr);

    void populate(
            const QString &archive_path,
            const std::vector<ArchiveFileInfo> &archive_files);

    ArchiveItem *rootItem;

    const QIcon m_icon_dir;
    QIcon m_icon_file;
};

#endif  // ARCHIVE_MODEL_H
