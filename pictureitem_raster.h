#ifndef PICTUREITEMRASTER_H
#define PICTUREITEMRASTER_H

#include "komicviewer_enums.h"
#include "pictureitem_shared.h"

#include <QtCore/qtimer.h>
#include <QtGui/qwidget.h>

#include <QtCore/qtconcurrentrun.h>
#include <QtCore/qfuturewatcher.h>
#include <QtCore/qtconcurrentmap.h>

class PictureItemRaster : public QWidget
{
    Q_OBJECT

public:
    PictureItemRaster(PictureItemShared *picItemShared, QWidget *parent = 0, Qt::WindowFlags f = 0 );
    void setRotation(qreal r);
    void setFile(const FileInfo &info);

private:
    QPixmap pixmap_edited;
    PictureItemShared *picItemShared;
    QFutureWatcher<QPixmap> *imageLoad;
    QPixmap pixmap;

public slots:
    void setZoom(qreal current, qreal previous);

private slots:
    void imageFinished(int);

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void zoomChanged();
    void imageChanged();

};

#endif // PICTUREITEMRASTER_H
