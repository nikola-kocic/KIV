#ifndef NESTEDMODEL_H
#define NESTEDMODEL_H

#include <QAbstractItemModel>
#include <QHash>

#include <memory>


template <class TIdentifier>
class NestedModelHandler {
public:
    virtual ~NestedModelHandler() {}
    virtual QAbstractItemModel* createChildModel(const QModelIndex& fsIndex) const = 0;
    virtual QAbstractItemModel* getParentModel() const = 0;
    virtual TIdentifier getParentIdentifierFromIndex(const QModelIndex &parentIndex) const = 0;
    virtual TIdentifier getNullIdentifier() const = 0;
    virtual QModelIndex getParentIndexFromIdentifier(const TIdentifier& identifier) const = 0;
    virtual QModelIndex getChildIndexFromIdentifier(const QAbstractItemModel* childModel, const TIdentifier& identifier) const = 0;
    virtual TIdentifier getChildIdentifierFromIndex(const QModelIndex& childIndex) const = 0;
    virtual bool shouldHaveChilModel(const QModelIndex& parentIndex) const = 0;
    virtual QModelIndex createChildIndex(const QAbstractItemModel* childModel, int arow, int acolumn, quintptr i) const = 0;
    virtual QModelIndex createParentIndex(const QAbstractItemModel* parentModel, int arow, int acolumn, quintptr i) const = 0;
    virtual int getColumnCount() const = 0;
};


template <class TIdentifier>
struct Identifiers {
    Identifiers(const TIdentifier& parentIdentifier, const TIdentifier& childIdentifier)
        : parentIdentifier(parentIdentifier)
        , childIdentifier(childIdentifier)
    {
    }
    TIdentifier parentIdentifier;
    TIdentifier childIdentifier;
};

template <class TIdentifier>
class NestedModel : public QAbstractItemModel {
protected:
    std::unique_ptr<const NestedModelHandler<TIdentifier>> mModelHandler;
    QAbstractItemModel* mParentModel;

    // Remember source model from index internalId. Pointers are borrowed.
    mutable QHash<quintptr, const QAbstractItemModel*> mSourceModels;

    // quintptr is internalId of parent. Pointers are owned.
    QHash<quintptr, const QAbstractItemModel*> mChildModels;

    // Maps parent index internalId to parent model identifier
    QHash<quintptr, TIdentifier> mParentIdentifiers;

public:
    NestedModel(std::unique_ptr<const NestedModelHandler<TIdentifier>> modelHandler)
        : mModelHandler(std::move(modelHandler))
        , mParentModel(mModelHandler->getParentModel())
        , mSourceModels(QHash<quintptr, const QAbstractItemModel*>())
        , mChildModels(QHash<quintptr, const QAbstractItemModel*>())
        , mParentIdentifiers(QHash<quintptr, TIdentifier>())
    {
        doConnect(mParentModel);
    }

    virtual ~NestedModel() {
        for (const QAbstractItemModel* childModel : mChildModels.values()) {
            delete childModel;
        }
        mChildModels.clear();
    }

    QModelIndex indexFromIdentifiers(const Identifiers<TIdentifier>& identifiers) {
        const QModelIndex parentIndex = mModelHandler->getParentIndexFromIdentifier(identifiers.parentIdentifier);
        const QModelIndex proxyIndex = mapFromSource(parentIndex);
        if (canFetchMore(proxyIndex)) {
            fetchMore(proxyIndex);
        }
        // Child model was created in fetchMore if needed
        const QAbstractItemModel* childModel = mChildModels.value(proxyIndex.internalId(), nullptr);
        if (childModel != nullptr) {
            QModelIndex childIndex = mModelHandler->getChildIndexFromIdentifier(childModel, identifiers.childIdentifier);
            if (childIndex.isValid()) {
                return mapFromSource(childIndex);
            }
        }
        return proxyIndex;
    }

    Identifiers<TIdentifier> identifiersFromIndex(const QModelIndex& proxyIndex) {
        const QModelIndex sourceIndex = mapToSource(proxyIndex);
        if (sourceIndex.model() == mParentModel) {
            const TIdentifier parentIdentifier = mModelHandler->getParentIdentifierFromIndex(sourceIndex);
            return Identifiers<TIdentifier>(parentIdentifier, mModelHandler->getNullIdentifier());
        } else {
            const TIdentifier childIdentifier = mModelHandler->getChildIdentifierFromIndex(sourceIndex);
            const TIdentifier parentIdentifier = getParentIdentifier(sourceIndex.model());
            return Identifiers<TIdentifier>(parentIdentifier, childIdentifier);
        }
    }

protected:
    void doConnect(const QAbstractItemModel* model) {
        connect(model, &QAbstractItemModel::rowsAboutToBeInserted,
                this, &NestedModel::_q_sourceRowsAboutToBeInserted);
        connect(model, &QAbstractItemModel::rowsInserted,
                this, &NestedModel::_q_sourceRowsInserted);
        connect(model, &QAbstractItemModel::rowsAboutToBeRemoved,
                this, &NestedModel::_q_sourceRowsAboutToBeRemoved);
        connect(model, &QAbstractItemModel::rowsRemoved,
                this, &NestedModel::_q_sourceRowsRemoved);
        connect(model, &QAbstractItemModel::rowsAboutToBeMoved,
                this, &NestedModel::_q_sourceRowsAboutToBeMoved);
        connect(model, &QAbstractItemModel::rowsMoved,
                this, &NestedModel::_q_sourceRowsMoved);
        connect(model, &QAbstractItemModel::columnsAboutToBeInserted,
                this, &NestedModel::_q_sourceColumnsAboutToBeInserted);
        connect(model, &QAbstractItemModel::columnsInserted,
                this, &NestedModel::_q_sourceColumnsInserted);
        connect(model, &QAbstractItemModel::columnsAboutToBeRemoved,
                this, &NestedModel::_q_sourceColumnsAboutToBeRemoved);
        connect(model, &QAbstractItemModel::columnsRemoved,
                this, &NestedModel::_q_sourceColumnsRemoved);
        connect(model, &QAbstractItemModel::columnsAboutToBeMoved,
                this, &NestedModel::_q_sourceColumnsAboutToBeMoved);
        connect(model, &QAbstractItemModel::columnsMoved,
                this, &NestedModel::_q_sourceColumnsMoved);
        connect(model, &QAbstractItemModel::modelAboutToBeReset,
                this, &NestedModel::_q_sourceModelAboutToBeReset);
        connect(model, &QAbstractItemModel::modelReset,
                this, &NestedModel::_q_sourceModelReset);
        connect(model, &QAbstractItemModel::dataChanged,
                this, &NestedModel::_q_sourceDataChanged);
        connect(model, &QAbstractItemModel::headerDataChanged,
                this, &NestedModel::_q_sourceHeaderDataChanged);
        connect(model, &QAbstractItemModel::layoutAboutToBeChanged,
                this, &NestedModel::_q_sourceLayoutAboutToBeChanged);
        connect(model, &QAbstractItemModel::layoutChanged,
                this, &NestedModel::_q_sourceLayoutChanged);
    }

public:
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const {
        if (!sourceIndex.isValid()) {
            return QModelIndex();
        }

        const quintptr internalId = sourceIndex.internalId();
        Q_ASSERT(internalId  != 0);

        if (mSourceModels.value(internalId , nullptr) == nullptr) {
            mSourceModels.insert(internalId , sourceIndex.model());
        }
        return createIndex(sourceIndex.row(), sourceIndex.column(), internalId );
    }

    const QAbstractItemModel* getSourceModel(quintptr internalId) const {
        return mSourceModels.value(internalId, nullptr);
    }

    QModelIndex mapToSource(const QModelIndex& proxyIndex) const {
        if (!proxyIndex.isValid()) {
            return QModelIndex();
        }

        Q_ASSERT(this == proxyIndex.model());

        const quintptr internalId = proxyIndex.internalId();
        const QAbstractItemModel* sourceModel = getSourceModel(internalId);
        if (isParentModelIndex(proxyIndex)) {
            const QModelIndex sourceIndex = mModelHandler->createParentIndex(
                        sourceModel, proxyIndex.row(), proxyIndex.column(), internalId);
            Q_ASSERT(sourceIndex.internalPointer() != nullptr);
            return sourceIndex;
        } else {
            const QModelIndex sourceIndex = mModelHandler->createChildIndex(
                        sourceModel, proxyIndex.row(), proxyIndex.column(), internalId);
            Q_ASSERT(sourceIndex.internalPointer() != nullptr);
            return sourceIndex;
        }
    }

    const QAbstractItemModel* getChildModel(quintptr parentInternalId) const {
        return mChildModels.value(parentInternalId, nullptr);
    }

    const TIdentifier getParentIdentifier(const QAbstractItemModel* childModel) const {
        const quintptr parentInternalId = mChildModels.key(childModel, 0);
        Q_ASSERT(parentInternalId != 0);
        const TIdentifier identifier = mParentIdentifiers.value(parentInternalId);
        return identifier;
    }

    QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const override {
        if (!parent.isValid()) {
            return mParentModel->index(row, column);
        }
        if (hasChildModel(parent)) {
            const QAbstractItemModel* childModel = getChildModel(parent.internalId());
            return mapFromSource(childModel->index(row, column));
        }
        const QModelIndex sourceParent = mapToSource(parent);
        const QModelIndex sourceIndex = sourceParent.model()->index(row, column, sourceParent);
        return mapFromSource(sourceIndex);
    }

    QModelIndex parent(const QModelIndex &proxyChild) const override {
        Q_ASSERT(this == proxyChild.model());
        const QModelIndex sourceChild = mapToSource(proxyChild);
        QModelIndex sourceParent = sourceChild.parent();
        if (!sourceParent.isValid() && !isParentModelIndex(proxyChild)) {
            const TIdentifier identifier = getParentIdentifier(sourceChild.model());
            sourceParent = mModelHandler->getParentIndexFromIdentifier(identifier);
        }
        return mapFromSource(sourceParent);
    }

    bool isParentModelIndex(const QModelIndex &proxyChild) const {
        Q_ASSERT(this == proxyChild.model());
        const QAbstractItemModel* sourceModel = getSourceModel(proxyChild.internalId());
        return sourceModel == mParentModel;
    }

    TIdentifier getParentIndexIdentifier(const QModelIndex &proxyIndex) const {
        Q_ASSERT(isParentModelIndex(proxyIndex));
        const QModelIndex sourceIndex = mapToSource(proxyIndex);
        Q_ASSERT(sourceIndex.model() == mParentModel);
        return mModelHandler->getParentIdentifierFromIndex(sourceIndex);
    }

    bool hasChildModel(const QModelIndex &proxyIndex) const {
        return mParentIdentifiers.contains(proxyIndex.internalId());
    }

    bool shouldHaveChildModel(const QModelIndex &proxyIndex) const {
        bool shouldHaveChildModel = false;
        if (isParentModelIndex(proxyIndex)) {
            const QModelIndex sourceIndex = mapToSource(proxyIndex);
            Q_ASSERT(sourceIndex.model() == mParentModel);
            shouldHaveChildModel = mModelHandler->shouldHaveChilModel(sourceIndex);
        }
        return shouldHaveChildModel;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        int rowCount = 0;
        if (!parent.isValid()) {
            rowCount = mParentModel->rowCount();
        } else if (hasChildModel(parent)) {
            const QAbstractItemModel *childModel = getChildModel(parent.internalId());
            rowCount = childModel->rowCount();
        } else {
            const QModelIndex sourceParent = mapToSource(parent);
            rowCount = sourceParent.model()->rowCount(sourceParent);
        }
        return rowCount;
    }

    // Column count must be the same in all models or else selection behaves weird
    int columnCount(const QModelIndex &/*parent*/ = QModelIndex()) const override {
        const int columnCount = mModelHandler->getColumnCount();
        return columnCount;
    }

    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override {
        const QModelIndex sourceIndex = mapToSource(proxyIndex);
        return sourceIndex.model()->data(sourceIndex, role);
    }

    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override {
        bool hasChildren = false;
        if (!parent.isValid()) {
            hasChildren = mParentModel->hasChildren();
        } else if (shouldHaveChildModel(parent)) {
            hasChildren = true;
        } else {
            const QModelIndex sourceParent = mapToSource(parent);
            hasChildren = sourceParent.model()->hasChildren(sourceParent);
        }
        return hasChildren;
    }

    bool canFetchMore(const QModelIndex &parent) const override {
        bool canFetchMore = false;
        if (shouldHaveChildModel(parent)) {
            const QAbstractItemModel* childModel = getChildModel(parent.internalId());
            canFetchMore = (childModel == nullptr);
        } else {
            const QModelIndex sourceParent = mapToSource(parent);
            if (sourceParent.isValid()) {
                canFetchMore = sourceParent.model()->canFetchMore(sourceParent);
            }
        }
        return canFetchMore;
    }

    void fetchMore(const QModelIndex &parent) override {
        if (shouldHaveChildModel(parent)) {
            const quintptr parentInternalId = parent.internalId();
            Q_ASSERT(mChildModels.value(parentInternalId, nullptr) == nullptr);

            const TIdentifier parentIdentifier = getParentIndexIdentifier(parent);
            const QAbstractItemModel* childModel = mModelHandler->createChildModel(parent);

            beginInsertRows(parent, 0, childModel->rowCount());
            mChildModels.insert(parentInternalId, childModel);
            mParentIdentifiers.insert(parentInternalId, parentIdentifier);
            endInsertRows();

            return;
        }

        const QModelIndex sourceParent = mapToSource(parent);
        if (sourceParent.model() == mParentModel) {
            mParentModel->fetchMore(sourceParent);
            return;
        }

        Q_ASSERT(false);
    }

protected:
    QList<QPersistentModelIndex> layoutChangePersistentIndexes;
    QModelIndexList proxyIndexes;

    void _q_sourceColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end) {
        beginInsertColumns(mapFromSource(parent), start, end);
    }
    void _q_sourceColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest) {
        beginMoveColumns(mapFromSource(sourceParent), sourceStart, sourceEnd, mapFromSource(destParent), dest);
    }
    void _q_sourceColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end) {
        beginRemoveColumns(mapFromSource(parent), start, end);
    }
    void _q_sourceColumnsInserted(const QModelIndex &/*parent*/, int /*start*/, int /*end*/) {
        endInsertColumns();
    }
    void _q_sourceColumnsMoved(const QModelIndex &/*sourceParent*/, int /*sourceStart*/, int /*sourceEnd*/, const QModelIndex &/*destParent*/, int /*dest*/) {
        endMoveColumns();
    }
    void _q_sourceColumnsRemoved(const QModelIndex &/*parent*/, int /*start*/, int /*end*/) {
        endRemoveColumns();
    }
    void _q_sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) {
        dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight), roles);
    }
    void _q_sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last) {
        headerDataChanged(orientation, first, last);
    }

    void _q_sourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint) {
        const auto proxyPersistentIndexes = persistentIndexList();
        for (const QPersistentModelIndex &proxyPersistentIndex : proxyPersistentIndexes) {
            proxyIndexes << proxyPersistentIndex;
            Q_ASSERT(proxyPersistentIndex.isValid());
            const QPersistentModelIndex srcPersistentIndex = mapToSource(proxyPersistentIndex);
            Q_ASSERT(srcPersistentIndex.isValid());
            layoutChangePersistentIndexes << srcPersistentIndex;
        }

        QList<QPersistentModelIndex> parents;
        parents.reserve(sourceParents.size());
        for (const QPersistentModelIndex &parent : sourceParents) {
            if (!parent.isValid()) {
                parents << QPersistentModelIndex();
                continue;
            }
            const QModelIndex mappedParent = mapFromSource(parent);
            Q_ASSERT(mappedParent.isValid());
            parents << mappedParent;
        }
        layoutAboutToBeChanged(sourceParents, hint);
    }

    void _q_sourceLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint) {
        for (int i = 0; i < proxyIndexes.size(); ++i) {
            changePersistentIndex(proxyIndexes.at(i), mapFromSource(layoutChangePersistentIndexes.at(i)));
        }

        layoutChangePersistentIndexes.clear();
        proxyIndexes.clear();

        QList<QPersistentModelIndex> parents;
        parents.reserve(sourceParents.size());
        for (const QPersistentModelIndex &parent : sourceParents) {
            if (!parent.isValid()) {
                parents << QPersistentModelIndex();
                continue;
            }
            const QModelIndex mappedParent = mapFromSource(parent);
            Q_ASSERT(mappedParent.isValid());
            parents << mappedParent;
        }
        layoutChanged(sourceParents, hint);
    }

    void _q_sourceModelAboutToBeReset() {
        beginResetModel();
    }
    void _q_sourceModelReset() {
        endResetModel();
    }
    void _q_sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end) {
        beginInsertRows(mapFromSource(parent), start, end);
    }
    void _q_sourceRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest) {
        beginMoveRows(mapFromSource(sourceParent), sourceStart, sourceEnd, mapFromSource(destParent), dest);
    }
    void _q_sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) {
        beginRemoveRows(mapFromSource(parent), start, end);
    }
    void _q_sourceRowsInserted(const QModelIndex &/*parent*/, int /*start*/, int /*end*/) {
        endInsertRows();
    }
    void _q_sourceRowsMoved(const QModelIndex &/*sourceParent*/, int /*sourceStart*/, int /*sourceEnd*/, const QModelIndex &/*destParent*/, int /*dest*/) {
        endMoveRows();
    }
    void _q_sourceRowsRemoved(const QModelIndex &/*parent*/, int /*start*/, int /*end*/) {
        endRemoveRows();
    }
};

#endif // NESTEDMODEL_H
