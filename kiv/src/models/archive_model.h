#ifndef ARCHIVE_MODEL_H
#define ARCHIVE_MODEL_H

#include <QAbstractItemModel>
#include <QIcon>
#include <QModelIndex>
#include <QVariant>

#include "kiv/include/IArchiveExtractor.h"
#include "kiv/src/models/archive_item.h"
#include "kiv/src/fileinfo.h"


class ArchiveModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit ArchiveModel(const IArchiveExtractor *const archive_extractor,
                          const QString &path,
                          QObject *const parent = 0);
    ~ArchiveModel() override;

    QVariant data(const QModelIndex &index, const int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(const int section,
                        const Qt::Orientation orientation,
                        const int role = Qt::DisplayRole) const override;
    QModelIndex index(const int row,
                      const int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex getDirectory(const QString &path);  // Use with info.getZipPath()
    QModelIndex findIndexChild(const QString &text,
                               const QModelIndex &root = QModelIndex());
    int getType() const;


private:
    ArchiveItem *getItem(const QModelIndex &index) const;
    ArchiveItem* AddNode(const QString &name,
                         const QDateTime &date,
                         const quint64 &bytes,
                         const QString &path,
                         const int type,
                         ArchiveItem *const parent = 0);

    void populate(const QString &archive_path,
                  const QList<const ArchiveFileInfo *> &archive_files);

    const IArchiveExtractor *const m_archive_extractor;
    ArchiveItem *rootItem;

    const QIcon m_icon_dir;
    QIcon m_icon_file;
};

#endif  // ARCHIVE_MODEL_H
