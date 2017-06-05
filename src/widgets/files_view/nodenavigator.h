#ifndef NODENAVIGATOR_H
#define NODENAVIGATOR_H

#include "include/optional.hpp"
#include "enums.h"
#include "nodeidentifier.h"

#include <QAbstractItemModel>

#include <functional>

class NodeNavigator : public QObject {
    Q_OBJECT

    template<typename T>
    using optional = std::experimental::optional<T>;

public:
    NodeNavigator(QAbstractItemModel* model, const INodeIdentifier* nodeIdentifier);

    void getNextImage(const QModelIndex &startIndex);
    void getPreviousImage(const QModelIndex &startIndex);
    void nodeLoaded(const QPersistentModelIndex &index);

signals:
    void navigated(QModelIndex index);

private:
    enum class Direction {
        NEXT,
        PREVIOUS
    };

    struct Cache {
        Cache(const QModelIndex& index, const QModelIndex& waitingFor, Direction direction, bool initial)
            : index(index), waitingFor(waitingFor), direction(direction), initial(initial)
        {}

        QPersistentModelIndex index;
        QPersistentModelIndex waitingFor;
        Direction direction;
        bool initial;
    };

    QAbstractItemModel* mModel;
    const INodeIdentifier* mNodeIdentifier;
    optional<Cache> mCache;

    bool isImageNode(const QModelIndex& index) const;
    void getImageRec(const QModelIndex &startIndex, Direction direction, bool initial);
};

#endif // NODENAVIGATOR_H
