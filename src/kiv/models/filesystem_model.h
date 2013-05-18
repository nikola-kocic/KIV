#ifndef FILESYSTEM_MODEL_H
#define FILESYSTEM_MODEL_H

#include <QFileSystemModel>

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit FileSystemModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // FILESYSTEM_MODEL_H
