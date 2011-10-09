#ifndef PIXMAPLOADER_H
#define PIXMAPLOADER_H

#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

#include <QtCore/qobject.h>
#include <QtGui/qpixmap.h>

class PixmapLoader : public QObject
{
    Q_OBJECT

public:
    PixmapLoader();
    void setFilePath(const QString &filepath, bool isZip = false, const QString &zipFileName = "");
    QString getFilePath();

private:
    QString filepath;
    bool isZip;
    QString zipFileName;
    void loadFromFile();
    void loadFromZip();

public slots:
    void loadPixmap();

signals:
    void finished(QPixmap);
};

#endif // PIXMAPLOADER_H
