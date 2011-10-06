#ifndef GENERATETHUMBNAIL_H
#define GENERATETHUMBNAIL_H

#include <QtCore/QThread>
#include <QtGui/QPixmap>
#include <QtGui/QImageReader>
#include <QtGui/qicon.h>

class generateThumbnail : public QObject
{
    Q_OBJECT
public:
    generateThumbnail(const QString& path, int length);

public slots:
    void returnThumbnail();

private:
    QString path;
    int length;

signals:
    void finished(QIcon);
};

#endif // GENERATETHUMBNAIL_H
