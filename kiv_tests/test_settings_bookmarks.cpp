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
    QCOMPARE(*(bookmarks.first()), *(expected.first()));
}

void TestSettingsBookmarks::testBookmarkAddDuplicate()
{
    bool addResult = m_settings->addBookmark("b1name", "B:\\img1.jpg");
    QCOMPARE(addResult, true);
    addResult = m_settings->addBookmark("b1name", "B:\\img1.jpg");
    QCOMPARE(addResult, false);
}

void TestSettingsBookmarks::testBookmarkDeleteFromMiddle()
{
    m_settings->addBookmark("b1name", "B:\\img1.jpg");
    m_settings->addBookmark("b1name", "B:\\img2.jpg");
    m_settings->addBookmark("b1name", "B:\\img3.jpg");

    bool delResult = m_settings->deleteBookmark("B:\\img2.jpg");
    QCOMPARE(delResult, true);
    QList<Bookmark*> bookmarks = m_settings->getBookmarks();
    QCOMPARE(bookmarks.size(), 2);
    QCOMPARE(m_settings->getBookmarkCount(), 2);
    QCOMPARE(bookmarks.at(0)->getPath(), QString("B:\\img1.jpg"));
    QCOMPARE(bookmarks.at(1)->getPath(), QString("B:\\img3.jpg"));
}

void TestSettingsBookmarks::testBookmarkDeleteFromEnd()
{
    m_settings->addBookmark("b1name", "B:\\img1.jpg");
    m_settings->addBookmark("b1name", "B:\\img2.jpg");
    m_settings->addBookmark("b1name", "B:\\img3.jpg");

    bool delResult = m_settings->deleteBookmark("B:\\img3.jpg");
    QCOMPARE(delResult, true);
    QList<Bookmark*> bookmarks = m_settings->getBookmarks();
    QCOMPARE(bookmarks.size(), 2);
    QCOMPARE(m_settings->getBookmarkCount(), 2);
    QCOMPARE(bookmarks.at(0)->getPath(), QString("B:\\img1.jpg"));
    QCOMPARE(bookmarks.at(1)->getPath(), QString("B:\\img2.jpg"));
}

void TestSettingsBookmarks::testBookmarkDeleteFromStart()
{
    m_settings->addBookmark("b1name", "B:\\img1.jpg");
    m_settings->addBookmark("b1name", "B:\\img2.jpg");
    m_settings->addBookmark("b1name", "B:\\img3.jpg");

    bool delResult = m_settings->deleteBookmark("B:\\img1.jpg");
    QCOMPARE(delResult, true);
    QList<Bookmark*> bookmarks = m_settings->getBookmarks();
    QCOMPARE(bookmarks.size(), 2);
    QCOMPARE(m_settings->getBookmarkCount(), 2);
    QCOMPARE(bookmarks.at(0)->getPath(), QString("B:\\img2.jpg"));
    QCOMPARE(bookmarks.at(1)->getPath(), QString("B:\\img3.jpg"));
}

void TestSettingsBookmarks::testBookmarkDeleteNotExisting()
{
    bool addResult = m_settings->addBookmark("b1name", "B:\\img1.jpg");
    QCOMPARE(addResult, true);
    bool delResult = m_settings->deleteBookmark("B:\\img2.jpg");
    QCOMPARE(delResult, false);
}
