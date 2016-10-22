#ifndef VIEW_FILES_TREE_H
#define VIEW_FILES_TREE_H

#include <QTreeView>



class TreeViewFiles : public QTreeView
{
    Q_OBJECT

public:
    explicit TreeViewFiles(QWidget *parent = nullptr);

protected slots:
    void rowsInserted(const QModelIndex &parent, int start, int end) override;

signals:
    void rowsInsertedSignal(const QModelIndexList &indexes);
};

#endif // VIEW_FILES_TREE_H

