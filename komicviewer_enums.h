#ifndef KOMICVIEWERENUMS_H
#define KOMICVIEWERENUMS_H

#include <Qt>
#include <QtCore/qstringlist.h>
#include <QtCore/qfileinfo.h>

namespace LockMode
{
    enum Mode { None, Autofit, FitWidth, FitHeight, Zoom };
}

namespace ViewMode
{
    enum Mode { None, Autofit, FitWidth, FitHeight, Zoom };
}

typedef struct {
    QString filePath;
    QString zipFile;
    int thumbSize;
}ZipInfo;

const int TYPE_FILE = 1001;
const int TYPE_DIR = 1002;
const int TYPE_ARCHIVE = 1003;

bool isImage(const QFileInfo &fi);
bool isArchive(const QFileInfo &fi);

const int ROLE_TYPE = Qt::UserRole + 1;
const int ROLE_ARCHIVE_FILE_NAME = Qt::UserRole + 2;
const int ROLE_THUMBNAIL = Qt::UserRole + 3;

int getArchiveNumberFromItem(int number);

int makeArchiveNumberForItem(int number);

#endif // KOMICVIEWERENUMS_H
