#include "sortcombobox.h"

SortComboBox::SortComboBox(const QList<ColumnSort> &items,
        QWidget *parent)
    : QComboBox(parent)
    , m_items(items)
{
    for (const ColumnSort &sort : m_items)
    {
        this->addItem(this->getSortText(sort));
    }
    connect(this, SIGNAL(currentIndexChanged(int)),
            this, SLOT(on_currentIndexChanged(int)));
}

SortComboBox::~SortComboBox()
{

}

void SortComboBox::on_currentIndexChanged(int index)
{
    const ColumnSort sort = m_items.at(index);
    emit currentSortChanged(sort);
}

QString SortComboBox::getSortText(ColumnSort sort) const
{
    switch(sort.getColumn())
    {
    case Column::Name:
        return sort.getOrder() == SortOrder::Asc ? tr("Name Ascending"):
                                                   tr("Name Descending");
    case Column::Date:
        return sort.getOrder() == SortOrder::Asc ? tr("Date Ascending"):
                                                   tr("Date Descending");
    case Column::Size:
        return sort.getOrder() == SortOrder::Asc ? tr("Size Ascending"):
                                                   tr("Size Descending");
    }
    return QString();
}

bool SortComboBox::setSort(ColumnSort sort)
{
    const int index = std::distance(
                m_items.constBegin(),
                std::find(m_items.constBegin(), m_items.constEnd(), sort));
    if (index == m_items.size())
    {
        return false;
    }
    else
    {
        this->setCurrentIndex(index);
        return true;
    }
}
