#include "kiv/src/helper.h"

#include <QFileSystemModel>
#include <QImageReader>
#include <QLocale>
#include <QProcess>

constexpr decltype(Helper::filtersArchive) Helper::filtersArchive;

const QStringList& Helper::getFiltersImage()
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

QString Helper::size(const qint64 bytes)
{
    // According to the Si standard KB is 1000 bytes, KiB is 1024
    // but on windows sizes are calculated by dividing by 1024 so we do what they do.
    const qint64 kib = 1024;
    const qint64 mib = 1024 * kib;
    const qint64 gib = 1024 * mib;
    const qint64 tib = 1024 * gib;
    if (bytes >= tib)
        return QFileSystemModel::tr("%1 TiB").arg(QLocale().toString(1.0 * bytes / tib, 'f', 3));
    else if (bytes >= gib)
        return QFileSystemModel::tr("%1 GiB").arg(QLocale().toString(1.0 * bytes / gib, 'f', 2));
    else if (bytes >= mib)
        return QFileSystemModel::tr("%1 MiB").arg(QLocale().toString(1.0 * bytes / mib, 'f', 1));
    else if (bytes >= kib)
        return QFileSystemModel::tr("%1 KiB").arg(QLocale().toString(1.0 * bytes / kib));
    else if (bytes == 0)
        return "";
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
int Helper::naturalCompare(const QString &s1, const QString &s2,  Qt::CaseSensitivity cs)
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


void Helper::showInFileBrowser(const QString& path)
{
#if defined(Q_OS_WIN)
    const QString explorer = "explorer";
    QStringList param;
    if (!QFileInfo(path).isDir())
        param << QLatin1String("/select,");
    param << QDir::toNativeSeparators(path);
    QProcess::startDetached(explorer, param);
#elif defined(Q_OS_MAC)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
                  .arg(path);
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#else
    // we cannot select a file here, because no file browser really supports it...
    const QFileInfo fileInfo(path);
    const QString folder = fileInfo.isDir() ? fileInfo.absoluteFilePath() : fileInfo.absolutePath();
    const QString app = QString("xdg-open \"%1\"").arg(folder);
    QProcess browserProc;
    bool success = browserProc.startDetached(app);
    const QString error = QString::fromLocal8Bit(browserProc.readAllStandardError());
    success = success && error.isEmpty();
#endif
}
