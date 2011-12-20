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

const int ROLE_TYPE = Qt::UserRole + 1000;
const int ROLE_FILE_DATE = Qt::UserRole + 1001;

const int TYPE_ARCHIVE_DIR = 1004;
const int TYPE_ARCHIVE_FILE = 1005;


inline bool isArchiveFile(const QFileInfo &fi)
{
    return FileInfo::getFiltersArchive().contains(fi.suffix().toLower());
}

inline bool isImageFile(const QFileInfo &fi)
{
    return FileInfo::getFiltersImage().contains(fi.suffix().toLower());
}

inline bool checkFileExtension(const QFileInfo &info)
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


#endif // HELPER_H

