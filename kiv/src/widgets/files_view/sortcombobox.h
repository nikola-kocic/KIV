#ifndef SORTCOMBOBOX_H
#define SORTCOMBOBOX_H

#include <QComboBox>

#include "kiv/src/enums.h"

class SortComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit SortComboBox(
            const QList<ColumnSort> &items,
            QWidget *parent = nullptr);
    ~SortComboBox();
    bool setSort(ColumnSort sort);

signals:
    void currentSortChanged(ColumnSort);

public slots:

protected:
    QList<ColumnSort> m_items;
    QString getSortText(ColumnSort sort);
protected slots:
    void on_currentIndexChanged(int index);
};

#endif // SORTCOMBOBOX_H
