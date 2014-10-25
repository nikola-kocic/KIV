#include <QApplication>
#include <QtTest>
#include "test_settings_bookmarks.h"
#include "test_pictureitem_data.h"
#include "test_picture_item_raster.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    int err = 0;
    {
        TestSettingsBookmarks testSettingsBookmarks;
        err = qMax(err, QTest::qExec(&testSettingsBookmarks, app.arguments()));
    }
    {
        TestPictureItemData testPictureItemData;
        err = qMax(err, QTest::qExec(&testPictureItemData, app.arguments()));
    }
    {
        TestPictureItemRaster testPictureItemRaster;
        err = qMax(err, QTest::qExec(&testPictureItemRaster, app.arguments()));
    }
    return err;
}