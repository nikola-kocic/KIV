#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>

class FileInfo
{
public:
    FileInfo();
    static FileInfo fromPath(const QString &);

    bool isZip() const;
    bool fileExists() const;
    bool isValidContainer() const;

    QString getFilePath() const;
    QString zipImagePath() const;
    QString getImageFileName() const;

    QString containerPath; /* Doesn't end with "/" */
    QString imageFileName;

    QString zipPath; /* Ends with "/" */
    QString zipImageFileName;
    int thumbSize;

};

#endif // FILEINFO_H
