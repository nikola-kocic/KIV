#ifndef LOADPIXMAP_H
#define LOADPIXMAP_H

#include <QtCore/qobject.h>
#include <QtGui/qpixmap.h>

class loadPixmap : public QObject
{
    Q_OBJECT
public:
    loadPixmap(const QString& path);
private:
    QString path;
signals:
    void finished(QPixmap);
public slots:
    void loadFromFile();

};

#endif // LOADPIXMAP_H
