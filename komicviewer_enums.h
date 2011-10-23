#ifndef KOMICVIEWERENUMS_H
#define KOMICVIEWERENUMS_H

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

class FileInfo
{
public:
    //Doesn't end with "/"
    QString containerPath;
    QString imageFileName;

    //Ends with "/"
    QString zipPathToImage;
    QString zipImageFileName;
    int thumbSize;

    FileInfo()
    {
        containerPath = "";
        imageFileName = "";
        zipPathToImage = "";
        zipImageFileName = "";
        thumbSize = 0;
    }
};



const int TYPE_FILE = 1001;
const int TYPE_DIR = 1002;
const int TYPE_ARCHIVE = 1003;

const int TYPE_ARCHIVE_DIR = 1004;
const int TYPE_ARCHIVE_FILE = 1005;

bool isImage(const QFileInfo &fi);
bool isArchive(const QFileInfo &fi);
bool checkFileExtension(const QFileInfo &fi);

const int ROLE_TYPE = Qt::UserRole + 1;
const int ROLE_ARCHIVE_FILE_NAME = Qt::UserRole + 2;
const int ROLE_THUMBNAIL = Qt::UserRole + 3;


#endif // KOMICVIEWERENUMS_H

