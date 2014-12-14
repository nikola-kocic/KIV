#ifndef SORTCOMBOBOX_H
#define SORTCOMBOBOX_H

#include <QComboBox>

#include "kiv/src/enums.h"

class SortComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit SortComboBox(
            const QList<SortDirection> &items,
            QWidget *parent = nullptr);
    ~SortComboBox();
    bool setSort(SortDirection sort);

signals:
    void currentSortChanged(SortDirection);

public slots:

protected:
    QList<SortDirection> m_items;
    QString getSortText(SortDirection sort);
protected slots:
    void on_currentIndexChanged(int index);
};

#endif // SORTCOMBOBOX_H
