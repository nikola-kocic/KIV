#include "nodeidentifier.h"
#include "helper.h"

NodeType NodeIdentifier::identify(const QModelIndex& index) const {
    bool ok = false;
    const int nodeTypeInt = index.data(Helper::ROLE_NODE_TYPE).toInt(&ok);
    Q_ASSERT(ok);
    return NodeType(nodeTypeInt);
}
