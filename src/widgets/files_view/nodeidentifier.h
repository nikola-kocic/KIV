#ifndef NODEIDENTIFIER_H
#define NODEIDENTIFIER_H

#include <QModelIndex>

#include "enums.h"
#include "helper.h"

class INodeIdentifier
{
public:
    virtual ~INodeIdentifier() {}
    virtual NodeType identify(const QModelIndex& index) const = 0;
};

class NodeIdentifier : public INodeIdentifier
{
public:
    NodeType identify(const QModelIndex& index) const override;
};

#endif // NODEIDENTIFIER_H
