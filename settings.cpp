#include "settings.h"

#include <QApplication>

Settings::Settings()
{
    m_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,  QApplication::organizationName(), QApplication::applicationName());

    m_middleClick = m_settings->value("Mouse/MiddleClick", 1).toInt();
    m_wheel = m_settings->value("Mouse/Wheel", 1).toInt();

    m_jumpToEnd           = m_settings->value("Behavior/JumpToBottom", false).toBool();
    m_scrollPageByWidth   = m_settings->value("Behavior/ScrollPageByWidth", false).toBool();
    m_rightToLeft         = m_settings->value("Behavior/RightToLeft", false).toBool();
    m_pageChangeTimeout   = m_settings->value("Behavior/PageChangeTimeout", 300).toInt();
    m_scrollChangesPage   = m_settings->value("Behavior/ScrollChangesPage", true).toBool();

    m_largeIcons          = m_settings->value("Interface/LargeIcons", false).toBool();
    m_lastPath            = m_settings->value("Interface/LastPath", "").toString();
    m_hardwareAcceleration= m_settings->value("Interface/HardwareAcceleration", false).toBool();
    m_thumbSize           = m_settings->value("Interface/ThumbnailSize", 200).toInt();

    m_calculateAverageColor = m_settings->value("Interface/CalculateAverageColor", false).toBool();

    for (int i = 0; ; ++i)
    {
        QVariant varName = m_settings->value("Bookmarks/" + QString::number(i) + "/Name");
        QVariant varPath = m_settings->value("Bookmarks/" + QString::number(i) + "/Path");
        if (varName.isValid() && varPath.isValid())
        {
            m_bookmarks.append(Bookmark(varName.toString(), varPath.toString()));
        }
        else
        {
            break;
        }
    }

}

int Settings::getMiddleClick() const
{
    return m_middleClick;
}

void Settings::setMiddleClick(int v)
{
    m_middleClick = v;
    m_settings->setValue("Mouse/MiddleClick", v);
}


int Settings::getWheel() const
{
    return m_wheel;
}

void Settings::setWheel(int v)
{
    m_wheel = v;
    m_settings->setValue("Mouse/Wheel", v);
}

bool Settings::getScrollPageByWidth() const
{
    return m_scrollPageByWidth;
}

void Settings::setScrollPageByWidth(bool b)
{
    m_scrollPageByWidth = b;
    m_settings->setValue("Behavior/ScrollPageByWidth", m_scrollPageByWidth);
}

bool Settings::getRightToLeft() const
{
    return m_rightToLeft;
}

void Settings::setRightToLeft(bool b)
{
    m_rightToLeft = b;
    m_settings->setValue("Behavior/RightToLeft", m_rightToLeft);
}

int Settings::getPageChangeTimeout() const
{
    return m_pageChangeTimeout;
}

void Settings::setPageChangeTimeout(int v)
{
    m_pageChangeTimeout = v;
    m_settings->setValue("Behavior/PageChangeTimeout", m_pageChangeTimeout);
}

bool Settings::getLargeIcons() const
{
    return m_largeIcons;
}

void Settings::setLargeIcons(bool b)
{
    m_largeIcons = b;
    m_settings->setValue("Interface/LargeIcons", m_largeIcons);
}

QString Settings::getLastPath() const
{
    return m_lastPath;
}

void Settings::setLastPath(QString s)
{
    m_lastPath = s;
    m_settings->setValue("Interface/LastPath", m_lastPath);
}

bool Settings::getJumpToEnd() const
{
    return m_jumpToEnd;
}

void Settings::setJumpToEnd(bool b)
{
    m_jumpToEnd = b;
    m_settings->setValue("Behavior/JumpToBottom", m_jumpToEnd);
}

bool Settings::getHardwareAcceleration() const
{
    return m_hardwareAcceleration;
}

void Settings::setHardwareAcceleration(bool b)
{
    m_hardwareAcceleration = b;
    m_settings->setValue("Interface/HardwareAcceleration", m_hardwareAcceleration);
}


int Settings::getThumbnailSize() const
{
    return m_thumbSize;
}

void Settings::setThumbnailSize(int v)
{
    m_thumbSize = v;
    m_settings->setValue("Interface/ThumbnailSize", m_thumbSize);
}

bool Settings::getScrollChangesPage() const
{
    return m_scrollChangesPage;
}

void Settings::setScrollChangesPage(bool b)
{
    m_scrollChangesPage = b;
    m_settings->setValue("Behavior/ScrollChangesPage", m_scrollChangesPage);

}

bool Settings::getCalculateAverageColor() const
{
    return m_calculateAverageColor;
}

void Settings::setCalculateAverageColor(bool b)
{
    m_calculateAverageColor = b;
    m_settings->setValue("Interface/CalculateAverageColor", m_calculateAverageColor);
}

void Settings::addBookmark(QString name, QString path)
{
    int oldcount = m_bookmarks.size();
    m_settings->setValue("Bookmarks/" + QString::number(oldcount) + "/Name", name);
    m_settings->setValue("Bookmarks/" + QString::number(oldcount) + "/Path", path);

    m_bookmarks.append(Bookmark(name, path));
}

void Settings::deleteBookmark(int index)
{
    m_bookmarks.removeAt(index);
    refreshBookmarks();
}

int Settings::getBookmarkCount()
{
    return m_bookmarks.size();
}

QList<Bookmark> Settings::getBookmarks()
{
    return m_bookmarks;
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
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < m_bookmarks.size(); ++i)
    {
        m_settings->setValue("Bookmarks/" + QString::number(i) + "/Name", m_bookmarks.at(i).getName());
        m_settings->setValue("Bookmarks/" + QString::number(i) + "/Path", m_bookmarks.at(i).getPath());
    }
}
