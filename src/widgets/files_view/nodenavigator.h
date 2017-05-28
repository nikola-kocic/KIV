#ifndef NODENAVIGATOR_H
#define NODENAVIGATOR_H

#include "include/optional.hpp"
#include "enums.h"
#include "nodeidentifier.h"

#include <QAbstractItemModel>

#include <functional>

class NodeNavigator
{
    template<typename T>
    using optional = std::experimental::optional<T>;

public:
    NodeNavigator(QAbstractItemModel* model, const INodeIdentifier* nodeIdentifier)
        : mModel(model)
        , mNodeIdentifier(nodeIdentifier)
    {}

    optional<QModelIndex> getNextImage(const QModelIndex &startIndex) const;
    optional<QModelIndex> getPreviousImage(const QModelIndex &startIndex) const;

private:
    enum class Direction {
        NEXT,
        PREVIOUS
    };

    QAbstractItemModel* mModel;
    const INodeIdentifier* mNodeIdentifier;

    bool isImageNode(const QModelIndex& index) const;
    optional<QModelIndex> getImageRec(const QModelIndex &startIndex, Direction direction, bool initial) const;
};

#endif // NODENAVIGATOR_H
