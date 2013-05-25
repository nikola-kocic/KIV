#include "settings.h"

#include <QApplication>

//#define SETTINGS_DEBUG
#ifdef SETTINGS_DEBUG
#include "helper.h"
#endif

Settings::Settings()
    : m_settings(new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName()))

    , m_middleClick(m_settings->value("Mouse/MiddleClick", 1).toInt())
    , m_wheel(m_settings->value("Mouse/Wheel", 1).toInt())

    , m_scrollPageByWidth(m_settings->value("Behavior/ScrollPageByWidth", false).toBool())
    , m_rightToLeft(m_settings->value("Behavior/RightToLeft", false).toBool())
    , m_scrollChangesPage(m_settings->value("Behavior/ScrollChangesPage", true).toBool())
    , m_pageChangeTimeout(m_settings->value("Behavior/PageChangeTimeout", 300).toInt())
    , m_jumpToEnd(m_settings->value("Behavior/JumpToBottom", false).toBool())

    , m_calculateAverageColor(m_settings->value("Interface/CalculateAverageColor", false).toBool())
    , m_hardwareAcceleration(m_settings->value("Interface/HardwareAcceleration", false).toBool())
    , m_thumbSize(m_settings->value("Interface/ThumbnailSize", QSize(100, 100)).toSize())
    , m_largeIcons(m_settings->value("Interface/LargeIcons", false).toBool())
    , m_lastPath(m_settings->value("Interface/LastPath", "").toString())
{
#ifdef SETTINGS_DEBUG
    DEBUGOUT << "filename" << m_settings->fileName();
#endif

    for (int i = 0; ; ++i)
    {
        QVariant varName = m_settings->value("Bookmarks/" + QString::number(i) + "/Name");
        QVariant varPath = m_settings->value("Bookmarks/" + QString::number(i) + "/Path");
        if (varName.isValid() && varPath.isValid())
        {
#ifdef SETTINGS_DEBUG
            DEBUGOUT << "append" << "Bookmarks/" + QString::number(i) + "/Name"
                     << varName.toString() <<  varPath.toString();
#endif
            m_bookmarks.append(new Bookmark(varName.toString(), varPath.toString()));
        }
        else
        {
            break;
        }
    }
}

Settings::~Settings()
{
    qDeleteAll(m_bookmarks);
    m_bookmarks.clear();
    delete m_settings;
}


void Settings::addBookmark(const QString &name, const QString &path)
{
    const int oldcount = m_bookmarks.size();
    m_settings->setValue("Bookmarks/" + QString::number(oldcount) + "/Name", name);
    m_settings->setValue("Bookmarks/" + QString::number(oldcount) + "/Path", path);

    m_bookmarks.append(new Bookmark(name, path));
}

void Settings::deleteBookmark(const int index)
{
    delete m_bookmarks.at(index);
    m_bookmarks.removeAt(index);
    refreshBookmarks();
}

void Settings::refreshBookmarks()
{
    for (int i = 0; ; ++i)
    {
        QVariant varName = m_settings->value("Bookmarks/" + QString::number(i) + "/Name");
        QVariant varPath = m_settings->value("Bookmarks/" + QString::number(i) + "/Path");
        if (varName.isValid() && varPath.isValid())
        {
            m_settings->remove("Bookmarks/" + QString::number(i));
#ifdef SETTINGS_DEBUG
            DEBUGOUT << "removing" << "Bookmarks/" + QString::number(i);
#endif
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < m_bookmarks.size(); ++i)
    {
        m_settings->setValue("Bookmarks/" + QString::number(i) + "/Name", m_bookmarks.at(i)->getName());
        m_settings->setValue("Bookmarks/" + QString::number(i) + "/Path", m_bookmarks.at(i)->getPath());
#ifdef SETTINGS_DEBUG
            DEBUGOUT << "adding" << "Bookmarks/" + QString::number(i)
                        + "/Name", m_bookmarks.at(i)->getName();
            DEBUGOUT << "adding" << "Bookmarks/" + QString::number(i)
                        + "/Path", m_bookmarks.at(i)->getPath();
#endif
    }
}
