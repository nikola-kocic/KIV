#include <QtTest>
#include "test_settings_bookmarks.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    int err = 0;
    {
        TestSettingsBookmarks testSettingsBookmarks;
        err = qMax(err, QTest::qExec(&testSettingsBookmarks, app.arguments()));
    }
    return err;
}
