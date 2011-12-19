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
    Bookmark (QString name, QString path) : m_name(name), m_path(path) {}

    QString getName() const { return m_name; }
    void setName(QString name) { m_name = name; }

    QString getPath() const { return m_path; }
    void setPath(QString path) { m_path = path; }

private:
    QString m_name;
    QString m_path;
};

class Settings
{

public:
    Settings();

    int getMiddleClick() const;
    void setMiddleClick(int);

    int getWheel() const;
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
    void setLastPath(QString path);

    bool getJumpToEnd() const;
    void setJumpToEnd(bool);

    bool getHardwareAcceleration() const;
    void setHardwareAcceleration(bool);


    QSize getThumbnailSize() const;
    void setThumbnailSize(QSize size);

    bool getScrollChangesPage() const;
    void setScrollChangesPage(bool);

    bool getCalculateAverageColor() const;
    void setCalculateAverageColor(bool);

    void addBookmark(QString name, QString path);
    void deleteBookmark(int index);
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
