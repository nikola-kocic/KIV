#ifndef SETTINGS_H
#define SETTINGS_H

#include "fileinfo.h"
#include "enums.h"

#include <QSettings>
#include <QSize>

class Bookmark
{
public:
    Bookmark (const QString &name, const QString &path)
        : m_name(name)
        , m_path(path)
    {}

    inline QString getName() const { return m_name; }
    inline QString getPath() const { return m_path; }

private:
    const QString m_name;
    const QString m_path;
};

class Settings
{

public:
    explicit Settings();
    ~Settings();

    /* Behaviour */

    int getMiddleClick() const;
    void setMiddleClick(const int v);

    int getWheel() const;
    void setWheel(const int v);

    bool getScrollPageByWidth() const;
    void setScrollPageByWidth(const bool b);

    bool getRightToLeft() const;
    void setRightToLeft(const bool b);

    bool getScrollChangesPage() const;
    void setScrollChangesPage(const bool b);

    int getPageChangeTimeout() const;
    void setPageChangeTimeout(const int v);

    bool getJumpToEnd() const;
    void setJumpToEnd(const bool b);

    /* End Behaviour */

    bool getCalculateAverageColor() const;
    void setCalculateAverageColor(const bool b);

    bool getHardwareAcceleration() const;
    void setHardwareAcceleration(const bool b);

    QSize getThumbnailSize() const;
    void setThumbnailSize(const QSize &size);

    bool getLargeIcons() const;
    void setLargeIcons(const bool b);

    QString getLastPath() const;
    void setLastPath(const QString &path);

    int getBookmarkCount() const;
    QList<Bookmark *> getBookmarks() const;

    void addBookmark(const QString &name, const QString &path);
    void deleteBookmark(const int index);

private:
    void refreshBookmarks();

    QSettings *m_settings;

    int m_middleClick;
    int m_wheel;

    bool m_scrollPageByWidth;
    bool m_rightToLeft;
    bool m_scrollChangesPage;
    int m_pageChangeTimeout;
    bool m_jumpToEnd;

    bool m_calculateAverageColor;

    bool m_hardwareAcceleration;
    QSize m_thumbSize;
    bool m_largeIcons;
    QString m_lastPath;

    QList<Bookmark *> m_bookmarks;
};


inline int Settings::getMiddleClick() const
{ return m_middleClick; }

inline void Settings::setMiddleClick(const int v)
{
    m_middleClick = v;
    m_settings->setValue("Mouse/MiddleClick", v);
}


inline int Settings::getWheel() const
{ return m_wheel; }

inline void Settings::setWheel(const int v)
{
    m_wheel = v;
    m_settings->setValue("Mouse/Wheel", v);
}


inline bool Settings::getScrollPageByWidth() const
{ return m_scrollPageByWidth; }

inline void Settings::setScrollPageByWidth(const bool b)
{
    m_scrollPageByWidth = b;
    m_settings->setValue("Behavior/ScrollPageByWidth", m_scrollPageByWidth);
}


inline bool Settings::getRightToLeft() const
{ return m_rightToLeft; }

inline void Settings::setRightToLeft(const bool b)
{
    m_rightToLeft = b;
    m_settings->setValue("Behavior/RightToLeft", m_rightToLeft);
}


inline bool Settings::getScrollChangesPage() const
{ return m_scrollChangesPage; }

inline void Settings::setScrollChangesPage(const bool b)
{
    m_scrollChangesPage = b;
    m_settings->setValue("Behavior/ScrollChangesPage", m_scrollChangesPage);
}


inline int Settings::getPageChangeTimeout() const
{ return m_pageChangeTimeout; }

inline void Settings::setPageChangeTimeout(const int v)
{
    m_pageChangeTimeout = v;
    m_settings->setValue("Behavior/PageChangeTimeout", m_pageChangeTimeout);
}


inline bool Settings::getJumpToEnd() const
{ return m_jumpToEnd; }

inline void Settings::setJumpToEnd(const bool b)
{
    m_jumpToEnd = b;
    m_settings->setValue("Behavior/JumpToBottom", m_jumpToEnd);
}


inline bool Settings::getCalculateAverageColor() const
{ return m_calculateAverageColor; }

inline void Settings::setCalculateAverageColor(const bool b)
{
    m_calculateAverageColor = b;
    m_settings->setValue("Interface/CalculateAverageColor", m_calculateAverageColor);
}


inline bool Settings::getHardwareAcceleration() const
{ return m_hardwareAcceleration; }

inline void Settings::setHardwareAcceleration(const bool b)
{
    m_hardwareAcceleration = b;
    m_settings->setValue("Interface/HardwareAcceleration", m_hardwareAcceleration);
}


inline QSize Settings::getThumbnailSize() const
{ return m_thumbSize; }

inline void Settings::setThumbnailSize(const QSize &size)
{
    m_thumbSize = size;
    m_settings->setValue("Interface/ThumbnailSize", m_thumbSize);
}


inline bool Settings::getLargeIcons() const
{ return m_largeIcons; }

inline void Settings::setLargeIcons(const bool b)
{
    m_largeIcons = b;
    m_settings->setValue("Interface/LargeIcons", m_largeIcons);
}


inline QString Settings::getLastPath() const
{ return m_lastPath; }

inline void Settings::setLastPath(const QString &path)
{
    m_lastPath = path;
    m_settings->setValue("Interface/LastPath", m_lastPath);
}


inline int Settings::getBookmarkCount() const
{ return m_bookmarks.size(); }

inline QList<Bookmark *> Settings::getBookmarks() const
{ return m_bookmarks; }

#endif // SETTINGS_H
