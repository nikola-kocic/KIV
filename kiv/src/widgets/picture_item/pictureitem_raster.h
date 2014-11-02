#ifndef PICTUREITEM_RASTER_H
#define PICTUREITEM_RASTER_H


#include <QWidget>
#include "kiv/src/widgets/picture_item/pictureitem_interface.h"

class PictureItemRaster : private QWidget, public PictureItemInterface
{
    Q_OBJECT

public:
    explicit PictureItemRaster(PictureItemData *data, QWidget* parent);
    ~PictureItemRaster() override;
    void setRotation(const qreal current, const qreal previous) override;
    void setZoom(const qreal current, const qreal previous) override;
    void setImage(const QImage &img) override;
    void setNullImage() override {}
    QWidget* getWidget();
    void paint(QPainter &p, const QPaintEvent * const event);

private:
    QPixmap m_pixmap;
    QPixmap m_pixmap_edited;

protected:
    void paintEvent(QPaintEvent *event) override;

};

#endif  // PICTUREITEM_RASTER_H
