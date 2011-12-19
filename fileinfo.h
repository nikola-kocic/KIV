#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include <QStringList>

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

    static QStringList getFiltersImage();
    static QStringList getFiltersArchive();

    QString containerPath; /* Doesn't end with "/" */
    QString imageFileName;

    QString zipPath; /* Ends with "/" */
    QString zipImageFileName;

private:
    static QStringList m_filters_archive;
    static QStringList m_filters_image;
};

#endif // FILEINFO_H
