#ifndef ARCHIVE_MODEL_H
#define ARCHIVE_MODEL_H

#include "picture_loader.h"

#include <QStandardItemModel>
#include <QFileIconProvider>

class FilesModel : public QStandardItemModel
{
    Q_OBJECT

public:
    FilesModel(QObject *parent = 0);
    void setPath(const FileInfo &info);
    QModelIndex getDirectory(const QString &path);
    QModelIndex findIndexChild(const QString &text, const QModelIndex &root = QModelIndex());
    QModelIndex findRootIndexChild(const QString &text);

private:
    QStandardItem* AddNode(QStandardItem *node, const QString &name, int type);
    int indexToInsertByName(QStandardItem *parent, const QString &name);
    QIcon m_icon_dir;
    QIcon m_icon_file;
};

#endif // ARCHIVE_MODEL_H
