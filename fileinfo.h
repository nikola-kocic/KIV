#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>

class FileInfo
{
public:
    FileInfo();

    QString containerPath; /* Doesn't end with "/" */
    QString imageFileName;

    QString zipPathToImage; /* Ends with "/" */
    QString zipImageFileName;
    int thumbSize;

    bool isZip() const;
    QString getFilePath() const;
    QString zipImagePath() const;
    QString getImageFileName() const;
};

#endif // FILEINFO_H
