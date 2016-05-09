#include "kiv/src/helper.h"

#include <QFileSystemModel>
#include <QImageReader>
#include <QLocale>
#include <QProcess>

#ifdef KIV_USE_DBUS
    #include <QtDBus/QtDBus>
#endif

namespace Helper {

void windowsExplorerOpen(const FileInfo& fi);
void osxFinderOpen(const FileInfo& fi);
bool xdgOpen(const FileInfo& fi);
#ifdef KIV_USE_DBUS
    bool dbusOpen(const FileInfo& fi);
    bool dbusOpenThunar(const FileInfo& fi);
#endif


const QStringList& getFiltersImage()
{
    static QStringList filters_image{};
    if (filters_image.isEmpty())
    {
        for (const auto &format : QImageReader::supportedImageFormats())
        {
            filters_image.append(QString::fromLatin1(format));
        }
    }
    return filters_image;
}

QString size(const qint64 bytes)
{
    // According to the Si standard KB is 1000 bytes, KiB is 1024
    // but on windows sizes are calculated by dividing by 1024 so we do what they do.
    constexpr qint64 kib = 1024;
    constexpr qint64 mib = 1024 * kib;
    constexpr qint64 gib = 1024 * mib;
    constexpr qint64 tib = 1024 * gib;
    if (bytes >= tib) {
        return QFileSystemModel::tr("%1 TiB").arg(QLocale().toString(1.0 * bytes / tib, 'f', 3));
    } else if (bytes >= gib) {
        return QFileSystemModel::tr("%1 GiB").arg(QLocale().toString(1.0 * bytes / gib, 'f', 2));
    } else if (bytes >= mib) {
        return QFileSystemModel::tr("%1 MiB").arg(QLocale().toString(1.0 * bytes / mib, 'f', 1));
    } else if (bytes >= kib) {
        return QFileSystemModel::tr("%1 KiB").arg(QLocale().toString(1.0 * bytes / kib, 'f', 1));
    } else if (bytes == 0) {
        return "";
    }
    return QFileSystemModel::tr("%1 bytes").arg(QLocale().toString(bytes));
}

static inline QChar getNextChar(const QString &s, int location)
{
    return (location < s.length()) ? s.at(location) : QChar();
}

/*!
    Natural number sort, skips spaces.

    Examples:
    1, 2, 10, 55, 100
    01.jpg, 2.jpg, 10.jpg

    Note on the algorithm:
    Only as many characters as necessary are looked at and at most they all
    are looked at once.

    Slower then QString::compare() (of course)
  */
int naturalCompare(const QString &s1, const QString &s2,  Qt::CaseSensitivity cs)
{
    for (int l1 = 0, l2 = 0; l1 <= s1.count() && l2 <= s2.count(); ++l1, ++l2) {
        // skip spaces, tabs and 0's
        QChar c1 = getNextChar(s1, l1);
        while (c1.isSpace())
            c1 = getNextChar(s1, ++l1);
        QChar c2 = getNextChar(s2, l2);
        while (c2.isSpace())
            c2 = getNextChar(s2, ++l2);

        if (c1.isDigit() && c2.isDigit()) {
            while (c1.digitValue() == 0)
                c1 = getNextChar(s1, ++l1);
            while (c2.digitValue() == 0)
                c2 = getNextChar(s2, ++l2);

            int lookAheadLocation1 = l1;
            int lookAheadLocation2 = l2;
            int currentReturnValue = 0;
            // find the last digit, setting currentReturnValue as we go if it isn't equal
            for (
                QChar lookAhead1 = c1, lookAhead2 = c2;
                (lookAheadLocation1 <= s1.length() && lookAheadLocation2 <= s2.length());
                lookAhead1 = getNextChar(s1, ++lookAheadLocation1),
                lookAhead2 = getNextChar(s2, ++lookAheadLocation2)
                ) {
                bool is1ADigit = !lookAhead1.isNull() && lookAhead1.isDigit();
                bool is2ADigit = !lookAhead2.isNull() && lookAhead2.isDigit();
                if (!is1ADigit && !is2ADigit)
                    break;
                if (!is1ADigit)
                    return -1;
                if (!is2ADigit)
                    return 1;
                if (currentReturnValue == 0) {
                    if (lookAhead1 < lookAhead2) {
                        currentReturnValue = -1;
                    } else if (lookAhead1 > lookAhead2) {
                        currentReturnValue = 1;
                    }
                }
            }
            if (currentReturnValue != 0)
                return currentReturnValue;
        }

        if (cs == Qt::CaseInsensitive) {
            if (!c1.isLower()) c1 = c1.toLower();
            if (!c2.isLower()) c2 = c2.toLower();
        }
        int r = QString::localeAwareCompare(c1, c2);
        if (r < 0)
            return -1;
        if (r > 0)
            return 1;
    }
    // The two strings are the same (02 == 2) so fall back to the normal sort
    return QString::compare(s1, s2, cs);
}

void windowsExplorerOpen(const FileInfo& fi)
{
    const QString containerPath = fi.getContainerPath();
    const QString explorer = "explorer";
    QStringList param;
    if (!QFileInfo(containerPath).isDir())
    {
        param << QLatin1String("/select,");
    }
    param << QDir::toNativeSeparators(containerPath);
    QProcess::startDetached(explorer, param);
}

void osxFinderOpen(const FileInfo& fi)
{
    const QString containerPath = fi.getContainerPath();
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
                  .arg(containerPath);
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
}

bool xdgOpen(const FileInfo& fi)
{
    const QString containerPath = fi.getContainerPath();
    // we cannot select a file here, because no file browser really supports it...
    const QFileInfo fileInfo(containerPath);
    const QString folder = fileInfo.isDir() ? fileInfo.absoluteFilePath() : fileInfo.absolutePath();
    const QString app = QString("xdg-open");
    const QStringList args(folder);
    const bool success = QProcess::startDetached(app, args);
    return success;
}

#ifdef KIV_USE_DBUS
bool dbusOpen(const FileInfo& fi)
{
    const QString filepath = fi.isInArchive() ? fi.getContainerPath() : fi.getPath();
    const QUrl fileurl = QUrl::fromLocalFile(filepath);
    const QStringList args(fileurl.toString());
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.FileManager1",
                                                      "/org/freedesktop/FileManager1",
                                                      "org.freedesktop.FileManager1",
                                                      "ShowItems");
    msg << args << "";
    const QDBusMessage response = QDBusConnection::sessionBus().call(msg);
    const bool success = (response.type() != QDBusMessage::MessageType::ErrorMessage);
    return success;
}

bool dbusOpenThunar(const FileInfo& fi)
{
    const QString filepath = fi.isInArchive() ? fi.getContainerPath() : fi.getPath();
    const QString containerPath = fi.getContainerPath();
    const QFileInfo containerFileInfo(containerPath);
    const QString folder = containerFileInfo.isDir() ? containerFileInfo.absoluteFilePath() : containerFileInfo.absolutePath();
    const QString filename = QFileInfo(filepath).fileName();

    QDBusMessage msg = QDBusMessage::createMethodCall("org.xfce.FileManager",
                                                      "/org/xfce/FileManager",
                                                      "org.xfce.FileManager",
                                                      "DisplayFolderAndSelect");
    msg << folder << filename << "" << "";
    const QDBusMessage response = QDBusConnection::sessionBus().call(msg);
    const bool success = (response.type() != QDBusMessage::MessageType::ErrorMessage);
    return success;
}
#endif

void showInFileBrowser(const FileInfo& fi)
{
#if defined(Q_OS_WIN)
    windowsExplorerOpen(fi);
#elif defined(Q_OS_MAC)
    osxFinderOpen(fi);
#else // Linux
#ifdef KIV_USE_DBUS
    // Prefer to use D-Bus because that way file selection works
    // TODO: Settings for order
    if (dbusOpenThunar(fi) == false && dbusOpen(fi) == false)
#endif // KIV_USE_DBUS
    {
        xdgOpen(fi);
    }
#endif
}
}
