#ifndef PICTUREITEM_RASTER_H
#define PICTUREITEM_RASTER_H

#include "pictureitem_interface.h"
#include <QWidget>

class PictureItemRaster : private QWidget, public PictureItemInterface
{
    Q_OBJECT

public:
    explicit PictureItemRaster(PictureItemData *data, QWidget* parent);
    ~PictureItemRaster();
    void setRotation(const qreal current, const qreal previous);
    void setZoom(const qreal current, const qreal previous);
    void setImage(const QImage &img);
    void setNullImage() {}
    QWidget* getWidget();

private:
    QSizeF m_img_size;
    QPixmap m_pixmap;
    QPixmap m_pixmap_edited;

protected:
    void paintEvent(QPaintEvent *event);

};

#endif // PICTUREITEM_RASTER_H
