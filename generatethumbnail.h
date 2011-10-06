#ifndef GENERATETHUMBNAIL_H
#define GENERATETHUMBNAIL_H

#include <QtCore/QThread>
#include <QtGui/QPixmap>
#include <QtGui/QImageReader>
#include <QtGui/qicon.h>

class IconInfo
{
public:
    int index;
    QIcon icon;
};

class generateThumbnail : public QObject
{
    Q_OBJECT
public:
    generateThumbnail(const QString& path, int length, int index);

public slots:
    void returnThumbnail();

private:
    QString path;
    int length;
    int index;

signals:
    void finished(IconInfo);
};

#endif // GENERATETHUMBNAIL_H
