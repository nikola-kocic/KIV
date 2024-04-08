#ifndef NODENAVIGATOR_H
#define NODENAVIGATOR_H

#include "enums.h"
#include "nodeidentifier.h"

#include <QAbstractItemModel>

#include <functional>
#include <optional>

class NodeNavigator : public QObject {
    Q_OBJECT

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
    std::optional<Cache> mCache{};

    bool isImageNode(const QModelIndex& index) const;
    void getImageRec(const QModelIndex &startIndex, Direction direction, bool initial);
};

#endif // NODENAVIGATOR_H
