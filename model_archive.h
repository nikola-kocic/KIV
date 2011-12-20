#ifndef ARCHIVE_MODEL_H
#define ARCHIVE_MODEL_H

#include "picture_loader.h"

#include <QStandardItemModel>
#include <QFileIconProvider>

class ArchiveFilesModel : public QStandardItemModel
{
    Q_OBJECT

public:
    ArchiveFilesModel(QObject *parent = 0);
    void setPath(const FileInfo &info);
    QModelIndex getDirectory(const QString &path);
    QModelIndex findIndexChild(const QString &text, const QModelIndex &root = QModelIndex());

private:
    QStandardItem* AddNode(QStandardItem *parent, const QString &name, const int type, const QDateTime &date, const quint64 size = 0);
    int indexToInsertByName(QStandardItem *parent, const QString &name);
    QIcon m_icon_dir;
    QIcon m_icon_file;
};

#endif // ARCHIVE_MODEL_H
