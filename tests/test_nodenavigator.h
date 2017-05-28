#ifndef TEST_NODENAVIGATOR_H
#define TEST_NODENAVIGATOR_H

#include "widgets/files_view/nodenavigator.h"

#include <QObject>
#include <QStandardItem>

class TestNodeNavigator : public QObject
{
    Q_OBJECT

private:
    QAbstractItemModel* mModel;
    const INodeIdentifier* mNodeIdentifier;
    QHash<QString, QModelIndex> mNodes;
    NodeNavigator* mNodeNavigator;

    QStandardItem *insertNode(QStandardItem *parent, QStandardItem *item);
    QModelIndex getNode(const QString& key) const;

private Q_SLOTS:
    void init();
    void cleanup();
    void testNavigateNextImage();
    void testNavigatePreviousImage();
    void testNavigateNextImageFromContainer();
    void testNavigateNextImageToParentContainer();
    void testNavigateNextImageToSiblingContainer();
    void testNavigateNextImageToSiblingContainerSubfolder();
    void testNavigateNextImageInNextArchive();
};

#endif // TEST_NODENAVIGATOR_H
