#ifndef PICTUREITEMDATA_H
#define PICTUREITEMDATA_H

#include "enums.h"

#include <QPoint>
#include <QColor>
#include <QRectF>

class PictureItemData
{
public:
    explicit PictureItemData();

    int setZoom(const qreal desiredZoom, qreal &zoomVal);
    qreal getZoom() const;

    void setRotation(const qreal r);
    qreal getRotation() const;

    bool isPixmapNull() const;
    void setPixmapNull(const bool value);
    void setImageSize(const QSize &size);

    QPointF getOffset() const;
    QSizeF getImageSizeOriginal() const;
    QSizeF getImageSizeTransformed() const;

    QRectF m_boundingRect;
    QColor m_color_clear;

    void avoidOutOfScreen(const QSize &widgetSize);
    void updateSize(const QSize &widgetSize);
    QPointF pointToOrigin(const QSizeF &pictureSize,
                          const QSize &widgetSize) const;

    int fitToScreen(const QSize &widgetSize, qreal &zoomVal) const;
    int fitWidth(const QSize &widgetSize, qreal &zoomVal) const;
    int fitHeight(const QSize &widgetSize, qreal &zoomVal) const;

    void drag(const QPoint &pt,
              const QSize &widgetSize);
    void beginDrag(const QPoint &pt);

protected:
    bool m_pixmapNull;
    qreal m_zoom_value;
    qreal m_rotation_value;
    QPointF m_offset;
    QPoint m_point_drag;
    QSizeF m_img_size_original;
    QSizeF m_img_size_transformed; // Image size after transformation.
                                   // e.g. rotation.
};

inline bool PictureItemData::isPixmapNull() const
{ return m_pixmapNull; }

inline void PictureItemData::setPixmapNull(const bool value)
{ m_pixmapNull = value; }

inline void PictureItemData::setImageSize(const QSize &size)
{
    m_img_size_original = size;
    m_img_size_transformed = size;
    m_boundingRect = QRectF(0, 0, size.width(), size.height());
}

inline qreal PictureItemData::getZoom() const
{ return m_zoom_value; }

inline qreal PictureItemData::getRotation() const
{ return m_rotation_value; }

inline QPointF PictureItemData::getOffset() const
{ return m_offset; }

inline QSizeF PictureItemData::getImageSizeOriginal() const
{ return m_img_size_original; }

inline QSizeF PictureItemData::getImageSizeTransformed() const
{ return m_img_size_transformed; }


#endif  // PICTUREITEMDATA_H
