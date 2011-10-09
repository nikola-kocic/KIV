#ifndef THUMBNAILGENERATOR_H
#define THUMBNAILGENERATOR_H

#include <QtCore/QThread>
#include <QtGui/QPixmap>
#include <QtGui/QImageReader>
#include <QtGui/qicon.h>

class ThumbnailGenerator : public QObject
{
    Q_OBJECT
public:
    ThumbnailGenerator();
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

#endif // THUMBNAILGENERATOR_H
