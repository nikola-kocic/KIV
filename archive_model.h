#ifndef ARCHIVE_MODEL_H
#define ARCHIVE_MODEL_H

#include "pixmap_loader.h"

#include <QtGui/qstandarditemmodel.h>
#include <QtGui/qfileiconprovider.h>

class ArchiveModel : public QStandardItemModel
{
    Q_OBJECT

public:
    ArchiveModel();
    void setArchiveName(const QString &filePath);

private:
    QStandardItem *root;
    QStringList archive_files;
    QStandardItem* AddNode(QStandardItem* node, QString name, int index);
};

#endif // ARCHIVE_MODEL_H
