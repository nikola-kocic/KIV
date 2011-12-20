#ifndef SETTINGS_H
#define SETTINGS_H

#include "fileinfo.h"

#include <QSettings>
#include <QSize>

class MiddleClickAction
{
public:
    static const int None = 0;
    static const int Fullscreen = 1;
    static const int AutoFit = 2;
    static const int ZoomReset = 3;
    static const int NextPage = 4;
    static const int Quit = 5;
    static const int Boss = 6;
};

class WheelAction
{
public:
    static const int None = 0;
    static const int Scroll = 1;
    static const int ChangePage = 2;
    static const int Zoom = 3;
};

class FileViewMode
{
public:
    static const int List = 0;
    static const int Details = 1;
    static const int Icons = 2;
};

class Bookmark
{
public:
    Bookmark (const QString &name, const QString &path) : m_name(name), m_path(path) {}
    QString getName() const { return m_name; }
    QString getPath() const { return m_path; }

private:
    const QString m_name;
    const QString m_path;
};

class Settings
{

public:
    Settings();

    int getMiddleClick() const;
    void setMiddleClick(const int v);

    int getWheel() const;
    void setWheel(const int v);

    bool getScrollPageByWidth() const;
    void setScrollPageByWidth(const bool b);

    bool getRightToLeft() const;
    void setRightToLeft(const bool b);

    int getPageChangeTimeout() const;
    void setPageChangeTimeout(const int v);

    bool getLargeIcons() const;
    void setLargeIcons(const bool b);

    QString getLastPath() const;
    void setLastPath(const QString &path);

    bool getJumpToEnd() const;
    void setJumpToEnd(const bool b);

    bool getHardwareAcceleration() const;
    void setHardwareAcceleration(const bool b);


    QSize getThumbnailSize() const;
    void setThumbnailSize(const QSize &size);

    bool getScrollChangesPage() const;
    void setScrollChangesPage(const bool b);

    bool getCalculateAverageColor() const;
    void setCalculateAverageColor(const bool b);

    void addBookmark(const QString &name, const QString &path);
    void deleteBookmark(const int index);
    int getBookmarkCount();
    QList<Bookmark> getBookmarks();

private:
    void refreshBookmarks();
    QSettings *m_settings;

    int m_middleClick;
    int m_wheel;

    bool m_largeIcons;
    QString m_lastPath;

    bool m_scrollPageByWidth;
    bool m_rightToLeft;
    int m_pageChangeTimeout;
    bool m_jumpToEnd;
    bool m_scrollChangesPage;

    bool m_hardwareAcceleration;
    QSize m_thumbSize;

    bool m_calculateAverageColor;

    QList<Bookmark> m_bookmarks;
};


#endif // SETTINGS_H
