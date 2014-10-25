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
    explicit ArchiveModel(IArchiveExtractor *archive_extractor,
                          const QString &path,
                          QObject *parent = 0);
    ~ArchiveModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row,
                      int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

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
                         ArchiveItem *parent = 0,
                         const bool updateData = false);

    template<class TFileInfo>
    void populate(const QString &archive_path,
                  const QList<TFileInfo> &archive_files);

    IArchiveExtractor *m_archive_extractor;
    ArchiveItem *rootItem;

    const QIcon m_icon_dir;
    QIcon m_icon_file;
};

#endif  // ARCHIVE_MODEL_H
