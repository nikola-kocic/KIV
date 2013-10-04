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

    QPointF getOffset() const;

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
};

inline bool PictureItemData::isPixmapNull() const
{ return m_pixmapNull; }

inline void PictureItemData::setPixmapNull(const bool value)
{ m_pixmapNull = value; }

inline qreal PictureItemData::getZoom() const
{ return m_zoom_value; }

inline qreal PictureItemData::getRotation() const
{ return m_rotation_value; }

inline void PictureItemData::setRotation(const qreal r)
{ m_rotation_value = r; }

inline QPointF PictureItemData::getOffset() const
{ return m_offset; }

#endif  // PICTUREITEMDATA_H
