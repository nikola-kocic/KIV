#ifndef GENERATETHUMBNAIL_H
#define GENERATETHUMBNAIL_H

#include <QtCore/QThread>
#include <QtGui/QPixmap>
#include <QtGui/QImageReader>
#include <QtGui/qicon.h>

class thumbnailGenerator : public QObject
{
    Q_OBJECT
public:
    thumbnailGenerator();
    void setPath(const QString& path);
    QString getPath();
    void setLength(const int& length);
    int getLength();

public slots:
    void returnThumbnail();

private:
    QString path;
    int length;

signals:
    void finished(QIcon);
};

#endif // GENERATETHUMBNAIL_H
