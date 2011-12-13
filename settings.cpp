#include "settings.h"

#include <QApplication>
#include <QImageReader>


Settings::Settings()
{
    m_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,  QApplication::organizationName(), QApplication::applicationName());

    m_filters_archive << "zip" << "cbz";
    for (int i = 0; i < QImageReader::supportedImageFormats().size(); ++i)
    {
        m_filters_image.append(QImageReader::supportedImageFormats().at(i));
    }

    m_middleClick = (MiddleClick::Action)m_settings->value("Mouse/MiddleClick", 1).toInt();
    m_wheel = (Wheel::Action)m_settings->value("Mouse/Wheel", 1).toInt();

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
}

Settings *Settings::Instance()
{
    static Settings inst;
    return &inst;
}


MiddleClick::Action Settings::getMiddleClick() const
{
    return m_middleClick;
}

void Settings::setMiddleClick(int v)
{
    m_middleClick = (MiddleClick::Action)v;
    m_settings->setValue("Mouse/MiddleClick", v);
}


Wheel::Action Settings::getWheel() const
{
    return m_wheel;
}

void Settings::setWheel(int v)
{
    m_wheel = (Wheel::Action)v;
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


QStringList Settings::getFiltersImage() const
{
    return m_filters_image;
}

QStringList Settings::getFiltersArchive() const
{
    return m_filters_archive;
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
