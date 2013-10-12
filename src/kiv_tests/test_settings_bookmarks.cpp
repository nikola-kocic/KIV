#include "test_settings_bookmarks.h"
#include <QtTest>

void TestSettingsBookmarks::init()
{
    m_qsettings = new QSettings(
                QSettings::IniFormat,
                QSettings::UserScope,
                "KIV_unittest",
                "KIV_unittest");
    m_qsettings->clear();
    m_settings = new Settings(m_qsettings);
}

void TestSettingsBookmarks::cleanup()
{
    m_qsettings->clear();
    delete m_qsettings;
}

void TestSettingsBookmarks::testBookmarkAdd()
{
    bool addResult = m_settings->addBookmark("b1name", "B:\\img1.jpg");
    QCOMPARE(addResult, true);
    QList<Bookmark*> bookmarks = m_settings->getBookmarks();
    QList<Bookmark*> expected;
    expected.append(new Bookmark("b1name", "B:\\img1.jpg"));
    QCOMPARE(bookmarks.size(), 1);
    QCOMPARE(*(bookmarks.at(0)), *(expected.at(0)));
}

void TestSettingsBookmarks::testBookmarkAddDuplicate()
{
    bool addResult = m_settings->addBookmark("b1name", "B:\\img1.jpg");
    QCOMPARE(addResult, true);
    addResult = m_settings->addBookmark("b1name", "B:\\img1.jpg");
    QCOMPARE(addResult, false);
}

void TestSettingsBookmarks::testBookmarkDelete()
{
    bool addResult = m_settings->addBookmark("b1name", "B:\\img1.jpg");
    QCOMPARE(addResult, true);
    bool delResult = m_settings->deleteBookmark("B:\\img1.jpg");
    QCOMPARE(delResult, true);
    QList<Bookmark*> bookmarks = m_settings->getBookmarks();
    QCOMPARE(bookmarks.size(), 0);
    QCOMPARE(m_settings->getBookmarkCount(), 0);
}

void TestSettingsBookmarks::testBookmarkDeleteNotExisting()
{
    bool addResult = m_settings->addBookmark("b1name", "B:\\img1.jpg");
    QCOMPARE(addResult, true);
    bool delResult = m_settings->deleteBookmark("B:\\img2.jpg");
    QCOMPARE(delResult, false);
}
