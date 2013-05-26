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

    void setZoom(const qreal z);
    qreal getZoom() const;

    void setRotation(const qreal r);
    qreal getRotation() const;

    void setLockMode(const int mode);
    int getLockMode() const;

    void setHardwareAcceleration(const bool b);
    bool getHardwareAcceleration() const;

    bool isPixmapNull() const;
    void setPixmapNull(const bool value);

    QRectF m_boundingRect;
    QColor m_color_clear;
    qreal m_offsetX;
    qreal m_offsetY;

    void avoidOutOfScreen(const QSize &widgetSize);
    void updateSize(const QSize &widgetSize);
    QPointF pointToOrigin(const qreal width,
                          const qreal height,
                          const QSize &widgetSize);

private:
    bool m_pixmapNull;
    qreal m_zoom_value;
    qreal m_rotation_value;
    int m_lockMode;
};

inline bool PictureItemData::isPixmapNull() const
{ return m_pixmapNull; }

inline void PictureItemData::setPixmapNull(const bool value)
{ m_pixmapNull = value; }

inline qreal PictureItemData::getZoom() const
{ return m_zoom_value; }

inline int PictureItemData::getLockMode() const
{ return m_lockMode; }

inline void PictureItemData::setLockMode(const int mode)
{ m_lockMode = mode; }

inline qreal PictureItemData::getRotation() const
{ return m_rotation_value; }

inline void PictureItemData::setRotation(const qreal r)
{ m_rotation_value = r; }

#endif  // PICTUREITEMDATA_H
