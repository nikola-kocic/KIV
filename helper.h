#ifndef HELPER_H
#define HELPER_H

#include "fileinfo.h"
#include <QStringList>
#include <QFileInfo>
#include <QApplication>

namespace LockMode
{
enum Mode { None, Autofit, FitWidth, FitHeight, Zoom };
}

class Helper
{
public:
    static const int ROLE_TYPE = Qt::UserRole + 1000;
    static const int ROLE_FILE_DATE = Qt::UserRole + 1001;

    static const int TYPE_ARCHIVE = 1003;
    static const int TYPE_ARCHIVE_DIR = 1004;
    static const int TYPE_ARCHIVE_FILE = 1005;

    static QStringList m_filters_archive;
    static QStringList m_filters_image;
    static QStringList getFiltersImage();
    static QStringList getFiltersArchive();


    static QString size(qint64 bytes);

    static inline bool isArchiveFile(const QFileInfo &fi)
    {
        return getFiltersArchive().contains(fi.suffix().toLower());
    }

    static inline bool isImageFile(const QFileInfo &fi)
    {
        return getFiltersImage().contains(fi.suffix().toLower());
    }

    static inline bool checkFileExtension(const QFileInfo &info)
    {
        if (isArchiveFile(info) || info.isDir() || isImageFile(info))
        {
            return true;
        }
        else
        {
            return false;
        }
    }


    static inline bool FuzzyCompare(double p1, double p2)
    {
        if (qFuzzyIsNull(p1)) {
            return qFuzzyIsNull(p2);
        } else if (qFuzzyIsNull(p2)) {
            return false;
        } else {
            return (qAbs(p1 - p2) <= 0.000000000001 * qMin(qAbs(p1), qAbs(p2)));
        }
    }

    static inline bool FuzzyCompare(float p1, float p2)
    {
        if (qFuzzyIsNull(p1)) {
            return qFuzzyIsNull(p2);
        } else if (qFuzzyIsNull(p2)) {
            return false;
        } else {
            return (qAbs(p1 - p2) <= 0.00001f * qMin(qAbs(p1), qAbs(p2)));
        }
    }

};

#endif // HELPER_H

