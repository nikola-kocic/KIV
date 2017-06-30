#include "settings.h"

#include <QCoreApplication>

//#define SETTINGS_DEBUG
#ifdef SETTINGS_DEBUG
#include "helper.h"
#endif

Settings::Settings(QSettings *settings)
    : m_settings((settings != nullptr) ?
                     settings :
                     new QSettings(QSettings::IniFormat,
                                   QSettings::UserScope,
                                   QCoreApplication::organizationName(),
                                   QCoreApplication::applicationName()))
    , m_thumbSize(
          m_settings->value("Interface/ThumbnailSize", QSize(100, 100)).toSize()
          )
    , m_lastPath(m_settings->value("Interface/LastPath", "").toString())
    , m_leftClick(
          m_settings->value("Mouse/LeftClick",
                            LeftClickAction::BeginDrag).toInt())
    , m_middleClick(
          m_settings->value("Mouse/MiddleClick",
                            MiddleClickAction::Fullscreen).toInt())
    , m_wheel(m_settings->value("Mouse/Wheel", WheelAction::ChangeImage).toInt())
    , m_zoomFilter(
          static_cast<ZoomFilter>(m_settings->value("Interface/ZoomFilter", false).toInt()))
    , m_rightToLeft(m_settings->value("Behavior/RightToLeft", false).toBool())

    , m_calculateAverageColor(
          m_settings->value("Interface/CalculateAverageColor", false).toBool())
    , m_hardwareAcceleration(
          m_settings->value("Interface/HardwareAcceleration", false).toBool())
    , m_largeIcons(m_settings->value("Interface/LargeIcons", false).toBool())
{
#ifdef SETTINGS_DEBUG
    DEBUGOUT << "filename" << m_settings->fileName();
#endif

    for (int i = 0; ; ++i)
    {
        QVariant varName = m_settings->value("Bookmarks/" + QString::number(i)
                                             + "/Name");
        QVariant varPath = m_settings->value("Bookmarks/" + QString::number(i)
                                             + "/Path");
        if (varName.isValid() && varPath.isValid())
        {
#ifdef SETTINGS_DEBUG
            DEBUGOUT << "append" << "Bookmarks/" + QString::number(i) + "/Name"
                     << varName.toString() <<  varPath.toString();
#endif
            m_bookmarks.append(new Bookmark(varName.toString(),
                                            varPath.toString()));
        }
        else
        {
            break;
        }
    }
}

Settings::~Settings()
{
    qDeleteAll(m_bookmarks);
    m_bookmarks.clear();
    delete m_settings;
}

SettingsData Settings::getData() const
{
    return SettingsData(
                getLeftClick(),
                getMiddleClick(),
                getWheel(),
                getThumbnailSize(),
                getZoomFilter(),
                getRightToLeft(),
                getCalculateAverageColor(),
                getHardwareAcceleration(),
                getLargeIcons());
}


bool Settings::addBookmark(const QString &name, const QString &path)
{
    // Don't allow multiple bookmarks with same path
    for (const Bookmark *const bookmark : m_bookmarks)
    {
        if (bookmark->getPath() == path)
        {
            return false;
        }
    }
    const int oldcount = m_bookmarks.size();
    m_settings->setValue("Bookmarks/" + QString::number(oldcount) + "/Name",
                         name);
    m_settings->setValue("Bookmarks/" + QString::number(oldcount) + "/Path",
                         path);

    m_bookmarks.append(new Bookmark(name, path));
    return true;
}

bool Settings::deleteBookmark(const QString &path)
{
    bool success = false;
    for (Bookmark *const bookmark : m_bookmarks)
    {
        if (bookmark->getPath() == path)
        {
            m_bookmarks.removeAll(bookmark);
            delete bookmark;
            refreshBookmarks();
            success = true;
        }
    }
    return success;
}

void Settings::refreshBookmarks()
{
    for (int i = 0; ; ++i)
    {
        const QVariant varName = m_settings->value(
                    "Bookmarks/" + QString::number(i) + "/Name");
        const QVariant varPath = m_settings->value(
                    "Bookmarks/" + QString::number(i) + "/Path");
        if (varName.isValid() && varPath.isValid())
        {
            m_settings->remove("Bookmarks/" + QString::number(i));
#ifdef SETTINGS_DEBUG
            DEBUGOUT << "removing" << "Bookmarks/" + QString::number(i);
#endif
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < m_bookmarks.size(); ++i)
    {
        m_settings->setValue("Bookmarks/" + QString::number(i) + "/Name",
                             m_bookmarks.at(i)->getName());
        m_settings->setValue("Bookmarks/" + QString::number(i) + "/Path",
                             m_bookmarks.at(i)->getPath());
#ifdef SETTINGS_DEBUG
            DEBUGOUT << "adding" << "Bookmarks/" + QString::number(i)
                        + "/Name", m_bookmarks.at(i)->getName();
            DEBUGOUT << "adding" << "Bookmarks/" + QString::number(i)
                        + "/Path", m_bookmarks.at(i)->getPath();
#endif
    }
}
