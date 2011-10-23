#ifndef ARCHIVE_MODEL_H
#define ARCHIVE_MODEL_H

#include "picture_loader.h"

#include <QtGui/qstandarditemmodel.h>
#include <QtGui/qfileiconprovider.h>

class ArchiveModel : public QStandardItemModel
{
    Q_OBJECT

public:
    void setPath(const FileInfo &info);

private:
    QStandardItem* AddNode(QStandardItem *node, const QString &name, int type);
};

#endif // ARCHIVE_MODEL_H
