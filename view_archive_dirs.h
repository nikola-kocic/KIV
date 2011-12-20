#ifndef VIEW_ARCHIVE_DIRS_H
#define VIEW_ARCHIVE_DIRS_H

#include <QSortFilterProxyModel>
#include <QTreeView>

class ViewArchiveDirs : public QTreeView
{
    Q_OBJECT
public:
    explicit ViewArchiveDirs(QWidget *parent = 0);
    void setModel(QAbstractItemModel *model);

public slots:
    void setCurrentIndexFromSource(const QModelIndex &index);

private:

    class MySortFilterProxyModel : public QSortFilterProxyModel
    {
    protected:
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
        bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
    };


    MySortFilterProxyModel *m_proxy;

private slots:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

signals:
    void currentRowChanged(const QModelIndex &);

};

#endif // VIEW_ARCHIVE_DIRS_H
