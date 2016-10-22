#ifndef HELPER_H
#define HELPER_H

#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QStringList>

#include <array>

#include "kiv/src/fileinfo.h"
#include "kiv/src/polyfill.h"

#define DEBUGOUT qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")\
                          << Q_FUNC_INFO << "\n\t"

namespace Helper
{
    template <typename... T>
    constexpr auto make_array(T&&... values) ->
            std::array<
                typename std::decay<
                    typename std::common_type<T...>::type>::type,
                sizeof...(T)> {
        return std::array<
            typename std::decay<
                typename std::common_type<T...>::type>::type,
            sizeof...(T)>{{std::forward<T>(values)...}};
    }

    template<class C, class T>
    auto contains(const C& v, const T& x)
    -> decltype(std::end(v), true)
    {
        return std::end(v) != std::find(std::begin(v), std::end(v), x);
    }

    constexpr int ROLE_FILE_DATE = Qt::UserRole + 1001;
    constexpr auto filtersArchive = make_array("zip", "cbz", "rar", "cbr");

    const QStringList& getFiltersImage();
    QString size(const qint64 bytes);

    bool isArchiveFile(const QFileInfo &fi);
    bool isImageFile(const QFileInfo &fi);
    bool checkFileExtension(const QFileInfo &info);

    bool FuzzyCompare(const double p1, const double p2);
    bool FuzzyCompare(const float p1, const float p2);

    int naturalCompare(
        const QString &s1, const QString &s2, Qt::CaseSensitivity cs);

    // Opens file browser (e.g. explorer) in file directory
    // and if possible, selects file.
    // Doesn't work reliably for files inside of archives.
    void showInFileBrowser(const FileInfo &fi);
}

inline bool Helper::isArchiveFile(const QFileInfo &fi)
{
    for (auto &ext : Helper::filtersArchive)
    {
        // TODO: Use MIME
        if (fi.suffix().toLower().endsWith(ext))
        {
            return true;
        }
    }
    return false;
}

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

