#ifndef ARCHIVE_MODEL_H
#define ARCHIVE_MODEL_H

#include "picture_loader.h"

#include <QtGui/qstandarditemmodel.h>
#include <QtGui/qfileiconprovider.h>

class FilesModel : public QStandardItemModel
{
    Q_OBJECT

public:
    void setPath(const FileInfo &info);
    QModelIndex getDirectory(const QString &path);
    QModelIndex findIndexChild(const QString &text, const QModelIndex &root = QModelIndex());
    QModelIndex findRootIndexChild(const QString &text);

private:
    QStandardItem* AddNode(QStandardItem *node, const QString &name, int type);
    int indexToInsertByName(QStandardItem *parent, const QString &name);
};

#endif // ARCHIVE_MODEL_H
