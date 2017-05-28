#include <QApplication>
#include <QtTest>
#include "test_settings_bookmarks.h"
#include "test_pictureitem_data.h"
#include "test_picture_item_raster.h"
#include "test_zoom_widget.h"
#include "test_zoom_widget_no_defaults.h"
#include "test_archive_file_list.h"
#include "test_archive_files.h"
#include "test_nodenavigator.h"

#include "fixtures.h"
#include "archiveextractor.h"

#include "init_test_data.h"


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
    {
        TestZoomWidget testZoomWidget;
        err = qMax(err, QTest::qExec(&testZoomWidget, app.arguments()));
    }
    {
        TestZoomWidgetNoDefaults testZoomWidget;
        err = qMax(err, QTest::qExec(&testZoomWidget, app.arguments()));
    }
    {
        TestNodeNavigator testNodeNavigator;
        err = qMax(err, QTest::qExec(&testNodeNavigator, app.arguments()));
    }

    const QString base_dir(
                QDir::cleanPath(QCoreApplication::applicationDirPath() +
                                QDir::separator() + "assets" +
                                QDir::separator() + "images"));
    DirStructureFixture dsf(
                DirStructureFixturePathsUnicodeOutsideBMP::getPath
                , DirStructureFixturePathsUnicodeOutsideBMP::getDirName
                , DirStructureFixtureDateTimes::getDateTime
                , DirStructureFixtureSize::getSize
                , base_dir
                );
//    InitTestData itd;
//    InitTestArchives ita = itd.makeTestData(dsf);
//    ita.createZip();
    const ArchiveExtractor archive_extractor;
    {
        TestArchiveFileListRar5 testArchiveFileListRar5(&archive_extractor, dsf, base_dir);
        err = qMax(err, QTest::qExec(&testArchiveFileListRar5, app.arguments()));
    }
    DirStructureFixture dsfZip(
                DirStructureFixturePathsUnicodeOutsideBMP::getPath
                , DirStructureFixturePathsUnicodeOutsideBMP::getDirName
                , DirStructureFixtureDateTimesLocal::getDateTime
                , DirStructureFixtureSize::getSize
                , base_dir
                );
    {
        TestArchiveFileListZip testArchiveFileListZip(&archive_extractor, dsfZip, base_dir);
        err = qMax(err, QTest::qExec(&testArchiveFileListZip, app.arguments()));
    }
    {
        TestArchiveFilesZip testArchiveFiles(&archive_extractor, dsfZip);
        err = qMax(err, QTest::qExec(&testArchiveFiles, app.arguments()));
    }
    DirStructureFixture dsfRar4(
                DirStructureFixturePathsUnicodeInsideBMP::getPath
                , DirStructureFixturePathsUnicodeInsideBMP::getDirName
                , DirStructureFixtureDateTimesLocal::getDateTime
                , DirStructureFixtureSizeUnicodeInsideBMP::getSize
                , base_dir
                );
//    ita = itd.makeTestData(dsfRar4);
//    ita.createRar4();
    {
        // TODO: Check rar4 file times behaviour
        TestArchiveFileListRar4 testArchiveFileListRar4(&archive_extractor, dsfRar4, base_dir);
        err = qMax(err, QTest::qExec(&testArchiveFileListRar4, app.arguments()));
    }
    {
        TestArchiveFilesRar4 testArchiveFilesRar4(&archive_extractor, dsfRar4);
        err = qMax(err, QTest::qExec(&testArchiveFilesRar4, app.arguments()));
    }
    return err;
}
