#ifndef ARCHIVEEXTRACTOR_H
#define ARCHIVEEXTRACTOR_H

#include "kiv/include/IArchiveExtractor.h"

#include <QObject>

class ArchiveExtractor : public QObject, public IArchiveExtractor
{
    Q_OBJECT
public:
    explicit ArchiveExtractor(QObject *parent = 0);

    virtual int getFileInfoList(const QString &path,
                                QList<ArchiveFileInfo> &list);
    virtual int extract(const QString &archiveName,
                        const QString &fileName,
                        const QString &newFileName);
    virtual int readFile(const QString &archiveName,
                         const QString &fileName,
                         QByteArray &buffer);
signals:

public slots:

};

#endif // ARCHIVEEXTRACTOR_H
