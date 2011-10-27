#include "settings.h"

#include <QtGui/qapplication.h>
#include <QtGui/qimagereader.h>

Settings::Settings()
{
    this->settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,  QApplication::organizationName(), QApplication::applicationName());

    this->filters_archive << "zip" << "cbz";

    foreach (const QByteArray &ext, QImageReader::supportedImageFormats())
    {
        this->filters_image << ext;
    }

    this->middleClickHash["none"]         = MiddleClick::None;
    this->middleClickHash["fullscreen"]   = MiddleClick::Fullscreen;
    this->middleClickHash["autofit"]      = MiddleClick::AutoFit;
    this->middleClickHash["zoomreset"]    = MiddleClick::ZoomReset;
    this->middleClickHash["followmouse"]  = MiddleClick::FollowMouse;
    this->middleClickHash["nextpage"]     = MiddleClick::NextPage;
    this->middleClick = getMiddleClickFromString(settings->value("Mouse/MiddleClick", "fullscreen").toString());

    this->wheelHash["none"]       = Wheel::None;
    this->wheelHash["scroll"]     = Wheel::Scroll;
    this->wheelHash["changepage"] = Wheel::ChangePage;
    this->wheelHash["zoom"]       = Wheel::Zoom;
    this->wheel = getWheelFromString(settings->value("Mouse/Wheel", "scroll").toString());

    this->JumpToEnd           = this->settings->value("Behavior/JumpToBottom",         false).toBool();
    this->ScrollPageByWidth   = this->settings->value("Behavior/ScrollPageByWidth",    false).toBool();
    this->RightToLeft         = this->settings->value("Behavior/RightToLeft",          false).toBool();
    this->PageChangeTimeout   = this->settings->value("Behavior/PageChangeTimeout",    "300").toInt();

    this->LargeIcons          = this->settings->value("Interface/LargeIcons", false).toBool();
    this->LastPath            = this->settings->value("Interface/LastPath", "").toString();
    this->HardwareAcceleration= this->settings->value("Interface/HardwareAcceleration", false).toBool();
    this->thumbSize           = this->settings->value("Interface/ThumbnailSize", "200").toInt();
}

Settings *Settings::Instance()
{
    static Settings inst;
    return &inst;
}


MiddleClick::Action Settings::getMiddleClick()
{
    return this->middleClick;
}

void Settings::setMiddleClick(MiddleClick::Action m)
{
    this->middleClick = m;
    settings->setValue("Mouse/MiddleClick", this->getStringFromMiddleClick(this->middleClick));
}

MiddleClick::Action Settings::getMiddleClickFromString(QString s)
{
    return this->middleClickHash.value(s, MiddleClick::None);
}

QString Settings::getStringFromMiddleClick(MiddleClick::Action m)
{
    return this->middleClickHash.key(m, "none");
}

Wheel::Action Settings::getWheel()
{
    return this->wheel;
}

void Settings::setWheel(Wheel::Action w)
{
    this->wheel = w;
    this->settings->setValue("Mouse/Wheel", this->getStringFromWheel(this->wheel));
}

Wheel::Action Settings::getWheelFromString(QString s)
{
    return this->wheelHash.value(s, Wheel::None);
}

QString Settings::getStringFromWheel(Wheel::Action w)
{
    return this->wheelHash.key(w, "none");
}

bool Settings::getScrollPageByWidth()
{
    return this->ScrollPageByWidth;
}

void Settings::setScrollPageByWidth(bool b)
{
    this->ScrollPageByWidth = b;
    this->settings->setValue("Behavior/ScrollPageByWidth", this->ScrollPageByWidth);
}

bool Settings::getRightToLeft()
{
    return this->RightToLeft;
}

void Settings::setRightToLeft(bool b)
{
    this->RightToLeft = b;
    this->settings->setValue("Behavior/RightToLeft", this->RightToLeft);
}

int Settings::getPageChangeTimeout()
{
    return PageChangeTimeout;
}

void Settings::setPageChangeTimeout(int v)
{
    this->PageChangeTimeout = v;
    this->settings->setValue("Behavior/PageChangeTimeout", this->PageChangeTimeout);
}

bool Settings::getLargeIcons()
{
    return this->LargeIcons;
}

void Settings::setLargeIcons(bool b)
{
    this->LargeIcons = b;
    this->settings->setValue("Interface/LargeIcons", this->LargeIcons);
}

QString Settings::getLastPath()
{
    return this->LastPath;
}

void Settings::setLastPath(QString s)
{
    this->LastPath = s;
    this->settings->setValue("Interface/LastPath", this->LastPath);
}

bool Settings::getJumpToEnd()
{
    return this->JumpToEnd;
}

void Settings::setJumpToEnd(bool b)
{
    this->JumpToEnd = b;
    this->settings->setValue("Behavior/JumpToBottom", this->JumpToEnd);
}

bool Settings::getHardwareAcceleration()
{
    return this->HardwareAcceleration;
}

void Settings::setHardwareAcceleration(bool b)
{
    this->HardwareAcceleration = b;
    this->settings->setValue("Interface/HardwareAcceleration", this->HardwareAcceleration);
}


QStringList Settings::getFiltersImage()
{
    return this->filters_image;
}

QStringList Settings::getFiltersArchive()
{
    return this->filters_archive;
}

int Settings::getThumbnailSize()
{
    return this->thumbSize;
}

void Settings::setThumbnailSize(int v)
{
    this->thumbSize = v;
}
