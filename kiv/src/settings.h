#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QSize>

#include "kiv/src/fileinfo.h"
#include "kiv/src/enums.h"

class Bookmark
{
public:
    Bookmark (const QString &name, const QString &path)
        : m_name(name)
        , m_path(path)
    {}

    inline QString getName() const { return m_name; }
    inline QString getPath() const { return m_path; }
    bool operator ==(const Bookmark &other) const
    {
        return (m_name == other.m_name &&
                m_path == other.m_path);
    }

private:
    const QString m_name;
    const QString m_path;
};

class Settings
{

public:
    explicit Settings(QSettings *settings = nullptr);
    ~Settings();

    /* Behaviour */

    int getMiddleClick() const;
    void setMiddleClick(const int v);

    int getWheel() const;
    void setWheel(const int v);

    bool getRightToLeft() const;
    void setRightToLeft(const bool b);

    /* End Behaviour */

    bool getCalculateAverageColor() const;
    void setCalculateAverageColor(const bool b);

    bool getHardwareAcceleration() const;
    void setHardwareAcceleration(const bool b);

    inline ZoomFilter getZoomFilter() const;
    void setZoomFilter(const ZoomFilter v);

    QSize getThumbnailSize() const;
    void setThumbnailSize(const QSize &size);

    bool getLargeIcons() const;
    void setLargeIcons(const bool b);

    QString getLastPath() const;
    void setLastPath(const QString &path);

    int getBookmarkCount() const;
    QList<Bookmark *> getBookmarks() const;

    bool addBookmark(const QString &name, const QString &path);
    bool deleteBookmark(const QString &path);

private:
    void refreshBookmarks();

    QSettings *m_settings;

    QSize m_thumbSize;
    QString m_lastPath;
    QList<Bookmark *> m_bookmarks;

    int m_middleClick;
    int m_wheel;
    int m_pageChangeTimeout;
    ZoomFilter m_zoomFilter;

    bool m_scrollPageByWidth;
    bool m_rightToLeft;
    bool m_scrollChangesPage;
    bool m_jumpToEnd;
    bool m_calculateAverageColor;
    bool m_hardwareAcceleration;
    bool m_largeIcons;
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


inline bool Settings::getRightToLeft() const
{ return m_rightToLeft; }

inline void Settings::setRightToLeft(const bool b)
{
    m_rightToLeft = b;
    m_settings->setValue("Behavior/RightToLeft", m_rightToLeft);
}


inline bool Settings::getCalculateAverageColor() const
{ return m_calculateAverageColor; }

inline void Settings::setCalculateAverageColor(const bool b)
{
    m_calculateAverageColor = b;
    m_settings->setValue("Interface/CalculateAverageColor",
                         m_calculateAverageColor);
}


inline bool Settings::getHardwareAcceleration() const
{ return m_hardwareAcceleration; }

inline void Settings::setHardwareAcceleration(const bool b)
{
    m_hardwareAcceleration = b;
    m_settings->setValue("Interface/HardwareAcceleration",
                         m_hardwareAcceleration);
}

inline ZoomFilter Settings::getZoomFilter() const
{ return m_zoomFilter; }

inline void Settings::setZoomFilter(const ZoomFilter v)
{
    m_zoomFilter = v;
    m_settings->setValue("Interface/ZoomFilter",
                         static_cast<int>(m_zoomFilter));
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

#endif  // SETTINGS_H
