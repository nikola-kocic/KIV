#include "test_nodenavigator.h"

#include <QtTest>
#include <QStandardItemModel>
#include <QSignalSpy>

using std::experimental::optional;

namespace QTest {
    template<>
    char *toString(const QModelIndex &index)
    {
        QByteArray ba = "QAbstractItemModel(";
        ba += index.data(Qt::DisplayRole).toString().toUtf8();
        ba += ")";
        return qstrdup(ba.data());
    }
}

class MockNodeIdentifier : public INodeIdentifier {
public:
    NodeType identify(const QModelIndex& index) const override {
        const QString text = index.data(Qt::DisplayRole).toString();
        const NodeType ret = text.endsWith(".zip") ? Archive : text.endsWith(".png") ? Image : Directory;
        return ret;
    }
};

QStandardItem* TestNodeNavigator::insertNode(QStandardItem* parent, QStandardItem* item) {
    parent->appendRow(item);
    const QString key = item->data(Qt::DisplayRole).toString();
    const QModelIndex itemIndex = item->index();
    Q_ASSERT_X(itemIndex.isValid(), "insertNode", QString(key + " has invalid index").toUtf8());
    Q_ASSERT_X(!mNodes.contains(key), "insertNode", QString(key + " already inserted").toUtf8());
    mNodes.insert(key, itemIndex);
    return item;
}

void TestNodeNavigator::init()
{
    QStandardItemModel* model = new QStandardItemModel();
    QStandardItem* parentItem = model->invisibleRootItem();

    QStandardItem* root = insertNode(parentItem, new QStandardItem("/"));
    {
        QStandardItem* d1 = insertNode(root, new QStandardItem("dir1"));
        {
            QStandardItem* d1_a1 = insertNode(d1, new QStandardItem("d1_a1.zip"));
            {
                insertNode(d1_a1, new QStandardItem("d1_a1_d1"));  // empty dir

                QStandardItem* d1_a1_d2 = insertNode(d1_a1, new QStandardItem("d1_a1_d2"));
                {
                    QStandardItem* d1_a1_d2_d1 = insertNode(d1_a1_d2, new QStandardItem("d1_a1_d1_d1"));
                    {
                        insertNode(d1_a1_d2_d1, new QStandardItem("d1_a1_d2_d1_i1.png"));
                        insertNode(d1_a1_d2_d1, new QStandardItem("d1_a1_d2_d1_i2.png"));
                        insertNode(d1_a1_d2_d1, new QStandardItem("d1_a1_d2_d1_i3.png"));
                    }
                    insertNode(d1_a1_d2, new QStandardItem("d1_a1_d2_i1.png"));
                    insertNode(d1_a1_d2, new QStandardItem("d1_a1_d2_i2.png"));
                    insertNode(d1_a1_d2, new QStandardItem("d1_a1_d2_i3.png"));
                }

                QStandardItem* d1_a1_d3 = insertNode(d1_a1, new QStandardItem("d1_a1_d3"));
                {
                    QStandardItem* d1_a1_d3_d1 = insertNode(d1_a1_d3, new QStandardItem("d1_a1_d3_d1"));
                    {
                        insertNode(d1_a1_d3_d1, new QStandardItem("d1_a1_d3_d1_i1.png"));
                        insertNode(d1_a1_d3_d1, new QStandardItem("d1_a1_d3_d1_i2.png"));
                        insertNode(d1_a1_d3_d1, new QStandardItem("d1_a1_d3_d1_i3.png"));
                    }
                    QStandardItem* d1_a1_d3_d2 = insertNode(d1_a1_d3, new QStandardItem("d1_a1_d3_d2"));
                    {
                        insertNode(d1_a1_d3_d2, new QStandardItem("d1_a1_d3_d2_i1.png"));
                        insertNode(d1_a1_d3_d2, new QStandardItem("d1_a1_d3_d2_i2.png"));
                        insertNode(d1_a1_d3_d2, new QStandardItem("d1_a1_d3_d2_i3.png"));
                    }
                    QStandardItem* d1_a1_d3_d3 = insertNode(d1_a1_d3, new QStandardItem("d1_a1_d3_d3"));
                    {
                        insertNode(d1_a1_d3_d3, new QStandardItem("d1_a1_d3_d3_i1.png"));
                        insertNode(d1_a1_d3_d3, new QStandardItem("d1_a1_d3_d3_i2.png"));
                        insertNode(d1_a1_d3_d3, new QStandardItem("d1_a1_d3_d3_i3.png"));
                    }
                    insertNode(d1_a1_d3, new QStandardItem("d1_a1_d3_i1.png"));
                    insertNode(d1_a1_d3, new QStandardItem("d1_a1_d3_i2.png"));
                    insertNode(d1_a1_d3, new QStandardItem("d1_a1_d3_i3.png"));
                }

                QStandardItem* d1_a1_d4 = insertNode(d1_a1, new QStandardItem("d1_a1_d4"));
                {
                    insertNode(d1_a1_d4, new QStandardItem("d1_a1_d4_i1.png"));
                    insertNode(d1_a1_d4, new QStandardItem("d1_a1_d4_i2.png"));
                    insertNode(d1_a1_d4, new QStandardItem("d1_a1_d4_i3.png"));
                }
                insertNode(d1_a1, new QStandardItem("d1_a1_i1.png"));
                insertNode(d1_a1, new QStandardItem("d1_a1_i2.png"));
                insertNode(d1_a1, new QStandardItem("d1_a1_i3.png"));
            }

            QStandardItem* d1_a2 = insertNode(d1, new QStandardItem("d1_a2.zip"));
            {
                QStandardItem* d1_a2_d1 = insertNode(d1_a2, new QStandardItem("d1_a2_d1"));
                {
                    insertNode(d1_a2_d1, new QStandardItem("d1_a2_d1_i1.png"));
                    insertNode(d1_a2_d1, new QStandardItem("d1_a2_d1_i2.png"));
                    insertNode(d1_a2_d1, new QStandardItem("d1_a2_d1_i3.png"));
                }
            }
        }

        QStandardItem* d2 = insertNode(root, new QStandardItem("dir2"));

        QStandardItem* d2_a1 = insertNode(root, new QStandardItem("dir2archive1.zip"));
    }

    mModel = model;
    mNodeIdentifier = new MockNodeIdentifier();
    mNodeNavigator = new NodeNavigator(mModel, mNodeIdentifier);
}

void TestNodeNavigator::cleanup()
{
    delete mNodeNavigator;
    delete mNodeIdentifier;
    delete mModel;
    mNodes.clear();
}

QModelIndex TestNodeNavigator::getNode(const QString& key) const {
    const QModelIndex index = mNodes.value(key);
    Q_ASSERT(index.isValid());
    return index;
}

QModelIndex getNextIndex(NodeNavigator* navigator, void (NodeNavigator::*function)(const QModelIndex&), const QModelIndex& index) {
    QSignalSpy spy(navigator, &NodeNavigator::navigated);
    (navigator->*function)(index);

    Q_ASSERT(spy.count() == 1); // make sure the signal was emitted exactly one time
    QList<QVariant> arguments = spy.takeFirst(); // take the first signal
    const QModelIndex nextImage = arguments.at(0).toModelIndex();
    return nextImage;
}

QModelIndex getNext(NodeNavigator* navigator, const QModelIndex& index) {
    return getNextIndex(navigator, &NodeNavigator::getNextImage, index);
}


QModelIndex getPrevious(NodeNavigator* navigator, const QModelIndex& index) {
    return getNextIndex(navigator, &NodeNavigator::getPreviousImage, index);
}

void TestNodeNavigator::testNavigateNextImage()
{
    const QModelIndex nextImage = getNext(mNodeNavigator, getNode("d1_a1_d2_d1_i1.png"));
    const QModelIndex expectedNextImage = getNode("d1_a1_d2_d1_i2.png");
    QCOMPARE(nextImage, expectedNextImage);
}

void TestNodeNavigator::testNavigatePreviousImage()
{
    const QModelIndex nextImage = getPrevious(mNodeNavigator, getNode("d1_a1_d2_d1_i2.png"));
    const QModelIndex expectedImage = getNode("d1_a1_d2_d1_i1.png");
    QCOMPARE(nextImage, expectedImage);
}

void TestNodeNavigator::testNavigateNextImageFromContainer()
{
    const QModelIndex expectedNextImage = getNode("d1_a1_d2_d1_i1.png");

    QModelIndex nextImage = getNext(mNodeNavigator, getNode("/"));
    QCOMPARE(nextImage, expectedNextImage);
    nextImage = getNext(mNodeNavigator, getNode("dir1"));
    QCOMPARE(nextImage, expectedNextImage);
    nextImage = getNext(mNodeNavigator, getNode("d1_a1.zip"));
    QCOMPARE(nextImage, expectedNextImage);
    nextImage = getNext(mNodeNavigator, getNode("d1_a1_d1"));
    QCOMPARE(nextImage, expectedNextImage);
    nextImage = getNext(mNodeNavigator, getNode("d1_a1_d2"));
    QCOMPARE(nextImage, expectedNextImage);
    nextImage = getNext(mNodeNavigator, getNode("d1_a1_d1_d1"));
    QCOMPARE(nextImage, expectedNextImage);
}

void TestNodeNavigator::testNavigateNextImageToParentContainer()
{
    const QModelIndex nextImage = getNext(mNodeNavigator, getNode("d1_a1_d2_d1_i3.png"));
    const QModelIndex expectedNextImage = getNode("d1_a1_d2_i1.png");
    QCOMPARE(nextImage, expectedNextImage);
}

void TestNodeNavigator::testNavigateNextImageToSiblingContainer()
{
    const QModelIndex nextImage = getNext(mNodeNavigator, getNode("d1_a1_d3_d1_i3.png"));
    const QModelIndex expectedNextImage = getNode("d1_a1_d3_d2_i1.png");
    QCOMPARE(nextImage, expectedNextImage);
}

void TestNodeNavigator::testNavigateNextImageToSiblingContainerSubfolder()
{
    const QModelIndex nextImage = getNext(mNodeNavigator, getNode("d1_a1_d2_i3.png"));
    const QModelIndex expectedNextImage = getNode("d1_a1_d3_d1_i1.png");
    QCOMPARE(nextImage, expectedNextImage);
}

void TestNodeNavigator::testNavigateNextImageInNextArchive()
{
    const QModelIndex nextImage = getNext(mNodeNavigator, getNode("d1_a1_i3.png"));
    const QModelIndex expectedNextImage = getNode("d1_a2_d1_i1.png");
    QCOMPARE(nextImage, expectedNextImage);
}
