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
    void testZoomIn();
    void testZoomOut();
    void testRotate();

};

#endif // TEST_PICTUREITEM_DATA_H
