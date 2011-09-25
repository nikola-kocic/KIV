#include "settings.h"
#include <QtGui/qapplication.h>

Settings::Settings()
{
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,  QApplication::organizationName(), QApplication::applicationName());

    middleClickHash["none"] = MiddleClick::None;
    middleClickHash["fullscreen"] = MiddleClick::Fullscreen;
    middleClickHash["autofit"] = MiddleClick::AutoFit;
    middleClickHash["zoomreset"] = MiddleClick::ZoomReset;
    middleClickHash["followmouse"] = MiddleClick::FollowMouse;
    middleClickHash["nextpage"] = MiddleClick::NextPage;
    middleClick = getMiddleClickFromString(settings->value("Mouse/MiddleClick", "fullscreen").toString());

    wheelHash["none"] = Wheel::None;
    wheelHash["scroll"] = Wheel::Scroll;
    wheelHash["changepage"] = Wheel::ChangePage;
    wheelHash["zoom"] = Wheel::Zoom;
    wheel = getWheelFromString(settings->value("Mouse/Wheel", "scroll").toString());

    ScrollPageByWidth = settings->value("Behavior/ScrollPageByWidth", false).toBool();
    RightToLeft = settings->value("Behavior/RightToLeft", false).toBool();
    PageChangeTimeout = settings->value("Behavior/PageChangeTimeout", "300").toInt();

    LargeIcons = settings->value("Interface/LargeIcons", false).toBool();
    LastPath = settings->value("Interface/LastPath", "").toString();
    JumpToEnd = settings->value("Behavior/JumpToBottom", false).toBool();
}

Settings *Settings::Instance()
{
    static Settings inst;
    return &inst;
}


MiddleClick::Action Settings::getMiddleClick()
{
    return middleClick;
}

void Settings::setMiddleClick(MiddleClick::Action m)
{
    middleClick = m;
    settings->setValue("Mouse/MiddleClick", getStringFromMiddleClick(middleClick));
}


MiddleClick::Action Settings::getMiddleClickFromString(QString s)
{
    return middleClickHash.value(s, MiddleClick::None);
}

QString Settings::getStringFromMiddleClick(MiddleClick::Action m)
{
    return middleClickHash.key(m, "none");
}



Wheel::Action Settings::getWheel()
{
    return wheel;
}

void Settings::setWheel(Wheel::Action w)
{
    wheel = w;
    settings->setValue("Mouse/Wheel", getStringFromWheel(wheel));
}

Wheel::Action Settings::getWheelFromString(QString s)
{
    return wheelHash.value(s, Wheel::None);
}

QString Settings::getStringFromWheel(Wheel::Action w)
{
    return wheelHash.key(w, "none");
}



bool Settings::getScrollPageByWidth()
{
    return ScrollPageByWidth;
}
void Settings::setScrollPageByWidth(bool b)
{
    ScrollPageByWidth = b;
    settings->setValue("Behavior/ScrollPageByWidth", ScrollPageByWidth);
}

bool Settings::getRightToLeft()
{
    return RightToLeft;
}
void Settings::setRightToLeft(bool b)
{
    RightToLeft = b;
    settings->setValue("Behavior/RightToLeft", RightToLeft);
}

int Settings::getPageChangeTimeout()
{
    return PageChangeTimeout;
}

void Settings::setPageChangeTimeout(int v)
{
    PageChangeTimeout = v;
    settings->setValue("Behavior/PageChangeTimeout", PageChangeTimeout);
}

bool Settings::getLargeIcons()
{
    return LargeIcons;
}
void Settings::setLargeIcons(bool b)
{
    LargeIcons = b;
    settings->setValue("Interface/LargeIcons", LargeIcons);
}

QString Settings::getLastPath()
{
    return LastPath;
}
void Settings::setLastPath(QString s)
{
    LastPath = s;
    settings->setValue("Interface/LastPath", LastPath);
}

bool Settings::getJumpToEnd()
{
    return JumpToEnd;
}
void Settings::setJumpToEnd(bool b)
{
    JumpToEnd = b;
    settings->setValue("Behavior/JumpToBottom", JumpToEnd);
}
