#include "settings.h"

#include <QtGui/qapplication.h>
#include <QtGui/qimagereader.h>

Settings::Settings()
{
    this->settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,  QApplication::organizationName(), QApplication::applicationName());

    this->filters_archive << "zip" << "cbz";
    for (int i = 0; i < QImageReader::supportedImageFormats().size(); ++i)
    {
        this->filters_image.append(QImageReader::supportedImageFormats().at(i));
    }

    this->middleClick = (MiddleClick::Action)settings->value("Mouse/MiddleClick", 1).toInt();
    this->wheel = (Wheel::Action)settings->value("Mouse/Wheel", 1).toInt();

    this->jumpToEnd           = this->settings->value("Behavior/JumpToBottom", false).toBool();
    this->scrollPageByWidth   = this->settings->value("Behavior/ScrollPageByWidth", false).toBool();
    this->rightToLeft         = this->settings->value("Behavior/RightToLeft", false).toBool();
    this->pageChangeTimeout   = this->settings->value("Behavior/PageChangeTimeout", 300).toInt();
    this->scrollChangesPage   = this->settings->value("Behavior/ScrollChangesPage", true).toBool();

    this->largeIcons          = this->settings->value("Interface/LargeIcons", false).toBool();
    this->LastPath            = this->settings->value("Interface/LastPath", "").toString();
    this->HardwareAcceleration= this->settings->value("Interface/HardwareAcceleration", false).toBool();
    this->thumbSize           = this->settings->value("Interface/ThumbnailSize", 200).toInt();
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

void Settings::setMiddleClick(int v)
{
    this->middleClick = (MiddleClick::Action)v;
    settings->setValue("Mouse/MiddleClick", v);
}


Wheel::Action Settings::getWheel()
{
    return this->wheel;
}

void Settings::setWheel(int v)
{
    this->wheel = (Wheel::Action)v;
    this->settings->setValue("Mouse/Wheel", v);
}

bool Settings::getScrollPageByWidth()
{
    return this->scrollPageByWidth;
}

void Settings::setScrollPageByWidth(bool b)
{
    this->scrollPageByWidth = b;
    this->settings->setValue("Behavior/ScrollPageByWidth", this->scrollPageByWidth);
}

bool Settings::getRightToLeft()
{
    return this->rightToLeft;
}

void Settings::setRightToLeft(bool b)
{
    this->rightToLeft = b;
    this->settings->setValue("Behavior/RightToLeft", this->rightToLeft);
}

int Settings::getPageChangeTimeout()
{
    return pageChangeTimeout;
}

void Settings::setPageChangeTimeout(int v)
{
    this->pageChangeTimeout = v;
    this->settings->setValue("Behavior/PageChangeTimeout", this->pageChangeTimeout);
}

bool Settings::getLargeIcons()
{
    return this->largeIcons;
}

void Settings::setLargeIcons(bool b)
{
    this->largeIcons = b;
    this->settings->setValue("Interface/LargeIcons", this->largeIcons);
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
    return this->jumpToEnd;
}

void Settings::setJumpToEnd(bool b)
{
    this->jumpToEnd = b;
    this->settings->setValue("Behavior/JumpToBottom", this->jumpToEnd);
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
    this->settings->setValue("Interface/ThumbnailSize", this->thumbSize);
}

bool Settings::getScrollChangesPage()
{
    return this->scrollChangesPage;
}

void Settings::setScrollChangesPage(bool b)
{
    this->scrollChangesPage = b;
    this->settings->setValue("Behavior/ScrollChangesPage", this->scrollChangesPage);

}
