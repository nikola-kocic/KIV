#ifndef SORTCOMBOBOX_H
#define SORTCOMBOBOX_H

#include <QComboBox>

#include "enums.h"

class SortComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit SortComboBox(
            const QList<ColumnSort> &items,
            QWidget *parent = nullptr);
    bool setSort(ColumnSort sort);

signals:
    void currentSortChanged(ColumnSort);

protected:
    QList<ColumnSort> m_items;
    QString getSortText(ColumnSort sort) const;

protected slots:
    void on_currentIndexChanged(int index);
};

#endif // SORTCOMBOBOX_H
