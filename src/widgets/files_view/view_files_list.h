#ifndef VIEW_FILES_LIST_H
#define VIEW_FILES_LIST_H

#include <QListView>

#include "enums.h"

class ListViewFiles : public QListView
{
    Q_OBJECT

public:
    explicit ListViewFiles(QWidget *parent = nullptr);

    void setViewMode(const FileViewMode mode);

protected:
    void rowsInserted(const QModelIndex &parent, int start, int end) override;

signals:
    void rowsInsertedSignal(const QModelIndexList &indexes);
};

#endif // VIEW_FILES_LIST_H

