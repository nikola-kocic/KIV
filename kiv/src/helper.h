#ifndef HELPER_H
#define HELPER_H

#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QStringList>

#include "kiv/src/fileinfo.h"

#define DEBUGOUT qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")\
                          << Q_FUNC_INFO << "\n\t"

class Helper
{
public:
    static const int ROLE_FILE_DATE = Qt::UserRole + 1001;


    static QStringList getFiltersImage();

    static QStringList filtersArchive;

    static QString size(const qint64 bytes);

    static bool isArchiveFile(const QFileInfo &fi);
    static bool isImageFile(const QFileInfo &fi);
    static bool checkFileExtension(const QFileInfo &info);

    static bool FuzzyCompare(const double p1, const double p2);
    static bool FuzzyCompare(const float p1, const float p2);

    static int naturalCompare(const QString &s1,
                              const QString &s2,
                              Qt::CaseSensitivity cs);

    // Opens file browser (e.g. explorer) in file directory
    // and if possible, selects file.
    // Doesn't work reliably for files inside of archives.
    static void showInFileBrowser(const QString &path);

private:
    static QStringList m_filters_image;
};

inline bool Helper::isArchiveFile(const QFileInfo &fi)
{ return Helper::filtersArchive.contains(fi.suffix().toLower()); }

inline bool Helper::isImageFile(const QFileInfo &fi)
{ return getFiltersImage().contains(fi.suffix().toLower()); }

inline bool Helper::checkFileExtension(const QFileInfo &info)
{ return (isArchiveFile(info) || info.isDir() || isImageFile(info)); }


inline bool Helper::FuzzyCompare(const double p1, const double p2)
{
    if (qFuzzyIsNull(p1)) {
        return qFuzzyIsNull(p2);
    } else if (qFuzzyIsNull(p2)) {
        return false;
    } else {
        return (qAbs(p1 - p2) <= 0.000000000001 * qMin(qAbs(p1), qAbs(p2)));
    }
}

inline bool Helper::FuzzyCompare(const float p1, const float p2)
{
    if (qFuzzyIsNull(p1)) {
        return qFuzzyIsNull(p2);
    } else if (qFuzzyIsNull(p2)) {
        return false;
    } else {
        return (qAbs(p1 - p2) <= 0.00001f * qMin(qAbs(p1), qAbs(p2)));
    }
}

#endif  // HELPER_H

