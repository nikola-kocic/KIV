#ifndef SETTINGS_H
#define SETTINGS_H

#include "fileinfo.h"

#include <QtCore/qsettings.h>
#include <QtCore/qhash.h>
#include <QtCore/qstringlist.h>

namespace MiddleClick
{
enum Action {
    None = 0,
    Fullscreen = 1,
    AutoFit = 2,
    ZoomReset = 3,
    NextPage = 4,
    Quit = 5,
    Boss = 6
//    ,FollowMouse = 7
};
}

namespace Wheel
{
enum Action {
    None,
    Scroll,
    ChangePage,
    Zoom
};
}

class Settings{

public:
    static Settings  *Instance();

    MiddleClick::Action getMiddleClick() const;
    void setMiddleClick(int);

    Wheel::Action getWheel() const;
    void setWheel(int);

    bool getScrollPageByWidth() const;
    void setScrollPageByWidth(bool);

    bool getRightToLeft() const;
    void setRightToLeft(bool);

    int getPageChangeTimeout() const;
    void setPageChangeTimeout(int);

    bool getLargeIcons() const;
    void setLargeIcons(bool);

    QString getLastPath() const;
    void setLastPath(QString);

    bool getJumpToEnd() const;
    void setJumpToEnd(bool);

    bool getHardwareAcceleration() const;
    void setHardwareAcceleration(bool);

    QStringList getFiltersImage() const;
    QStringList getFiltersArchive() const;

    int getThumbnailSize() const;
    void setThumbnailSize(int);

    bool getScrollChangesPage() const;
    void setScrollChangesPage(bool);

    bool getCalculateAverageColor() const;
    void setCalculateAverageColor(bool);

private:

    /* Make your constructors private */
    Settings();
    Settings(const Settings&);
    Settings& operator= (const Settings&);

    QSettings *m_settings;

    MiddleClick::Action m_middleClick;
    Wheel::Action m_wheel;

    bool m_largeIcons;
    QString m_lastPath;
    QStringList m_filters_image;
    QStringList m_filters_archive;

    bool m_scrollPageByWidth;
    bool m_rightToLeft;
    int m_pageChangeTimeout;
    bool m_jumpToEnd;
    bool m_scrollChangesPage;

    bool m_hardwareAcceleration;
    int m_thumbSize;

    bool m_calculateAverageColor;
};


#endif // SETTINGS_H
