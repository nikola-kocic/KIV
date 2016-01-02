#ifndef PICTUREITEMDATA_H
#define PICTUREITEMDATA_H

#include "kiv/src/enums.h"

#include <QPoint>
#include <QColor>
#include <QRectF>

class PictureItemData
{
public:
    explicit PictureItemData();

    QRectF getBoundingRect() const;

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

    QColor m_color_clear;

    void avoidOutOfScreen();
    void updateOffsets();
    void updateSize();
    int resetImagePosition(bool rtl);

    int fitToScreen(qreal &zoomVal) const;
    int fitWidth(qreal &zoomVal) const;
    int fitHeight(qreal &zoomVal) const;

    void drag(const QPoint &pt);
    void beginDrag(const QPoint &pt);

    void setWidgetSize(const QSize &size);

protected:
    /* Source rectangle with negative coordinates and calculated zoom.
     * Real source rectangle can be calculated as:
     * QRectF(-m_boundingRect.topLeft() / m_zoom_value,
     *        m_boundingRect.size() / m_zoom_value)
    */
    QRectF m_boundingRect;

    /* Top-left coordinates of centered image (positive).
     * Used only when bounding rectangle size is smaller than widget size.
     * Else has value (0, 0).
    */
    QPointF m_offset;

    bool m_pixmapNull;
    qreal m_zoom_value;
    qreal m_rotation_value;
    QPointF m_point_drag;
    QSize m_widget_size;
    QSizeF m_img_size_original;
    QSizeF m_img_size_transformed; // Image size after transformation.
                                   // e.g. rotation.
    QPointF pointToOrigin() const;
};

inline QRectF PictureItemData::getBoundingRect() const
{ return m_boundingRect; }

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

inline void PictureItemData::setWidgetSize(const QSize &size)
{ m_widget_size = size; }

inline QSizeF PictureItemData::getImageSizeOriginal() const
{ return m_img_size_original; }

inline QSizeF PictureItemData::getImageSizeTransformed() const
{ return m_img_size_transformed; }


#endif  // PICTUREITEMDATA_H
