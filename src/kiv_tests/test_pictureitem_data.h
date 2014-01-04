#ifndef TEST_PICTUREITEM_DATA_H
#define TEST_PICTUREITEM_DATA_H

#include <QObject>

#include "../kiv/widgets/picture_item/pictureitem_data.h"

class TestPictureItemData : public QObject
{
    Q_OBJECT

private:
    PictureItemData p_data;

private Q_SLOTS:
    void init();
    void testZoomIn_FittedBefore_NotAfter();
    void testZoomIn_FittedPerfectlyBefore();
    void testZoomIn_DidntFitBefore();
    void testZoomIn_FitsAfter();
    void testZoomIn_DidntFitBefore_WasMoved();
    void testZoomOut();
    void testRotate();
    void testZoomOutRotate();

};

#endif // TEST_PICTUREITEM_DATA_H
