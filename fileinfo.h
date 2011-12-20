#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include <QStringList>
#include <QFileInfo>

class FileInfo
{
public:
    FileInfo();
    static FileInfo fromPath(const QString &);

    bool isArchive() const;

    bool fileExists() const;
    bool isValidContainer() const;

    QString getFilePath() const;
    QString zipImagePath() const;
    QString getImageFileName() const;

    QString getDebugInfo() const;

    static QStringList getFiltersImage();
    static QStringList getFiltersArchive();

    QFileInfo container; /* Folder must end with "/" */
    QFileInfo image;

    QString zipPath; /* Ends with "/" */
    QString zipImageFileName;

private:
    static QStringList m_filters_archive;
    static QStringList m_filters_image;
};

#endif // FILEINFO_H
