#include "nodenavigator.h"

#include <QDebug>

//#define DEBUG_NAVIGATION
#ifdef DEBUG_NAVIGATION
#include "helper.h"
#endif

using std::experimental::optional;

NodeNavigator::NodeNavigator(QAbstractItemModel *model, const INodeIdentifier *nodeIdentifier)
    : mModel(model)
    , mNodeIdentifier(nodeIdentifier)
    , mCache(optional<Cache>())
{
    Q_ASSERT(mModel != nullptr);
    Q_ASSERT(mNodeIdentifier != nullptr);
}

bool NodeNavigator::isImageNode(const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());
    return mNodeIdentifier->identify(index) == Image;
}

QString nodeText(const optional<QModelIndex>& index) {
    if (!index) {
        return "Index not valid";
    } else {
        return index.value().data(Qt::DisplayRole).toString();
    }
}

void NodeNavigator::nodeLoaded(const QPersistentModelIndex &index) {
    if (mCache) {
        const Cache cache = mCache.value();
        if (cache.waitingFor == index) {
            getImageRec(cache.index, cache.direction, cache.initial); // or false?
        }
    }
}

void NodeNavigator::getImageRec(const QModelIndex &index, Direction direction, bool initial)
{
    Q_ASSERT(index.isValid());
#ifdef DEBUG_NAVIGATION
    DEBUGOUT << "startIndex " << nodeText(index) << "; direction " << int(direction) << "; initial " << initial;
#endif
    if (isImageNode(index) && !initial) {
#ifdef DEBUG_NAVIGATION
    DEBUGOUT << "ret " << nodeText(index);
#endif
        mCache = optional<Cache>();
        emit navigated(index);
        return;
    }

    QModelIndex nextIndex = QModelIndex();
    const NodeType nodeType = mNodeIdentifier->identify(index);
    if ((nodeType == Directory || nodeType == Archive) && mModel->rowCount(index) == 0) {
        if (mModel->canFetchMore(index)) {
            DEBUGOUT << "fetchMore" << nodeText(index) << "; index " << index;
            mModel->fetchMore(index);
            if (mModel->rowCount(index) == 0 && nodeType == Directory) {
                return;
            }
        }
    }
    if (mModel->rowCount(index) == 0) {
#ifdef DEBUG_NAVIGATION
        DEBUGOUT << "rowCount is 0 for index" << nodeText(index);
#endif
        const int offset = (direction == Direction::NEXT ? 1 : -1);
        const QModelIndex sibling = index.sibling(index.row() + offset, false);
        if (sibling.isValid()) {
#ifdef DEBUG_NAVIGATION
            DEBUGOUT << "getting sibling";
#endif
            nextIndex = sibling;
        } else {
#ifdef DEBUG_NAVIGATION
            DEBUGOUT << "getting parent sibling";
#endif
            QModelIndex parent = index;
            QModelIndex parentSibling;

            do {
                parent = parent.parent();
                parentSibling = parent.sibling(parent.row() + offset, 0);
            } while (parent.isValid() && !parentSibling.isValid());

            if (parent.isValid()) {
                nextIndex = parentSibling;
            } else {
                mCache = optional<Cache>();
                emit navigated(QModelIndex());
                return;
            }
        }
    } else {
#ifdef DEBUG_NAVIGATION
        DEBUGOUT << "getting child";
#endif
        if (direction == Direction::NEXT) {
            nextIndex = index.child(0, 0);
        } else {
            nextIndex = index.child(mModel->rowCount(index) - 1, 0);
        }
    }
    Q_ASSERT(nextIndex.isValid());
    mCache = Cache(QPersistentModelIndex(index), QPersistentModelIndex(nextIndex), direction, initial);
    return getImageRec(nextIndex, direction, false);  // recursive
}

void NodeNavigator::getPreviousImage(const QModelIndex &startIndex)
{
    Q_ASSERT(startIndex.isValid());
#ifdef DEBUG_NAVIGATION
    DEBUGOUT << "startIndex " << nodeText(startIndex);
#endif
    getImageRec(startIndex, Direction::PREVIOUS, true);
}

void NodeNavigator::getNextImage(const QModelIndex &startIndex)
{
    Q_ASSERT(startIndex.isValid());
#ifdef DEBUG_NAVIGATION
    DEBUGOUT << "startIndex " << nodeText(startIndex);
#endif
    getImageRec(startIndex, Direction::NEXT, true);
}
