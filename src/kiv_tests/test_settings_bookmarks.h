#ifndef TEST_SETTINGS_BOOKMARKS_H
#define TEST_SETTINGS_BOOKMARKS_H

#include "../kiv/settings.h"

class TestSettingsBookmarks : public QObject
{
    Q_OBJECT

private:
    QSettings *m_qsettings ;
    Settings *m_settings;

private Q_SLOTS:
    void init();
    void cleanup();
    void testBookmarkAdd();
    void testBookmarkAddDuplicate();
    void testBookmarkDelete();
    void testBookmarkDeleteNotExisting();
};

#endif // TEST_SETTINGS_BOOKMARKS_H
