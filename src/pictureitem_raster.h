#ifndef PICTUREITEM_RASTER_H
#define PICTUREITEM_RASTER_H

#include "pictureitem.h"

#include <QWidget>

class PictureItem;

class PictureItemRaster : public QWidget
{
    Q_OBJECT

public:
    explicit PictureItemRaster(PictureItem* parent, Qt::WindowFlags f = 0);
    void setRotation(const qreal current, const qreal previous);
    void setFile(const FileInfo &info);
    void setZoom(const qreal current, const qreal previous);
    void setImage(const QImage &img);

private:
    PictureItem *m_picItem;
    QSizeF m_img_size;
    QPixmap m_pixmap;
    QPixmap m_pixmap_edited;

protected:
    void paintEvent(QPaintEvent *event);

};
#endif // PICTUREITEM_RASTER_H
