#include "sortcombobox.h"

SortComboBox::SortComboBox(
        const QList<SortDirection> &items,
        QWidget *parent)
    : QComboBox(parent)
    , m_items(items)
{
    for (const SortDirection &sort : m_items)
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
    const SortDirection sort = m_items.at(index);
    emit currentSortChanged(sort);
}

QString SortComboBox::getSortText(SortDirection sort)
{
    switch(sort)
    {
    case SortDirection::NameAsc:
        return tr("Name Ascending");
    case SortDirection::NameDesc:
        return tr("Name Descending");
    case SortDirection::DateAsc:
        return tr("Date Ascending");
    case SortDirection::DateDesc:
        return tr("Date Descending");
    case SortDirection::SizeAsc:
        return tr("Size Ascending");
    case SortDirection::SizeDesc:
        return tr("Size Descending");
    }
    return QString();
}

bool SortComboBox::setSort(SortDirection sort)
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
