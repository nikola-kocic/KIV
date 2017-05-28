#include "nodenavigator.h"

#include <QDebug>

//#define DEBUG_NAVIGATION
#ifdef DEBUG_NAVIGATION
#include "helper.h"
#endif

using std::experimental::optional;

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

optional<QModelIndex> NodeNavigator::getImageRec(const QModelIndex &index, Direction direction, bool initial) const
{
    Q_ASSERT(index.isValid());
#ifdef DEBUG_NAVIGATION
    DEBUGOUT << "startIndex " << nodeText(index);
#endif
    if (isImageNode(index) && !initial) {
        return index;
    }

    QModelIndex nextIndex = QModelIndex();
    if (mModel->rowCount(index) == 0) {
        if (mModel->canFetchMore(index)) {
            mModel->fetchMore(index);
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
                return optional<QModelIndex>();
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
    return getImageRec(nextIndex, direction, false);  // recursive
}

optional<QModelIndex> NodeNavigator::getPreviousImage(const QModelIndex &startIndex) const
{
    Q_ASSERT(startIndex.isValid());
#ifdef DEBUG_NAVIGATION
    DEBUGOUT << "startIndex " << nodeText(startIndex);
#endif
    const optional<QModelIndex> ret = getImageRec(startIndex, Direction::PREVIOUS, true);
#ifdef DEBUG_NAVIGATION
    DEBUGOUT << "ret = " << nodeText(ret);
#endif
    Q_ASSERT(!ret || isImageNode(ret.value()));
    return ret;
}

optional<QModelIndex> NodeNavigator::getNextImage(const QModelIndex &startIndex) const
{
    Q_ASSERT(startIndex.isValid());
#ifdef DEBUG_NAVIGATION
    DEBUGOUT << "startIndex " << nodeText(startIndex);
#endif
    const optional<QModelIndex> ret = getImageRec(startIndex, Direction::NEXT, true);
#ifdef DEBUG_NAVIGATION
    DEBUGOUT << "ret = " << nodeText(ret);
#endif
    Q_ASSERT(!ret || isImageNode(ret.value()));
    return ret;
}
