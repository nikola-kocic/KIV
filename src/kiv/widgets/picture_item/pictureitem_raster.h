#ifndef PICTUREITEM_RASTER_H
#define PICTUREITEM_RASTER_H


#include <QWidget>
#include "widgets/picture_item/pictureitem_interface.h"

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
    void paint(QPainter &p, const QPaintEvent * const event);

private:
    QPixmap m_pixmap;
    QPixmap m_pixmap_edited;

protected:
    void paintEvent(QPaintEvent *event);

};

#endif  // PICTUREITEM_RASTER_H
