#ifndef SETTINGSDATA_H
#define SETTINGSDATA_H

#include <QSize>
#include "enums.h"

class SettingsData
{
public:
    explicit SettingsData(
            int leftClick,
            int middleClick,
            int wheel,
            QSize thumbSize,
            ZoomFilter zoomFilter,
            bool rightToLeft,
            bool calculateAverageColor,
            bool hardwareAcceleration,
            bool largeIcons)
        : m_leftClick(leftClick)
        , m_middleClick(middleClick)
        , m_wheel(wheel)
        , m_thumbSize(thumbSize)
        , m_zoomFilter(zoomFilter)
        , m_rightToLeft(rightToLeft)
        , m_calculateAverageColor(calculateAverageColor)
        , m_hardwareAcceleration(hardwareAcceleration)
        , m_largeIcons(largeIcons)
    {
    }

    int getLeftClick() const {
        return m_leftClick;
    }
    int getMiddleClick() const {
        return m_middleClick;
    }
    int getWheel() const {
        return m_wheel;
    }
    bool getRightToLeft() const {
        return m_rightToLeft;
    }
    bool getCalculateAverageColor() const {
        return m_calculateAverageColor;
    }
    bool getHardwareAcceleration() const {
        return m_hardwareAcceleration;
    }
    ZoomFilter getZoomFilter() const {
        return m_zoomFilter;
    }
    QSize getThumbnailSize() const {
        return m_thumbSize;
    }
    bool getLargeIcons() const {
        return m_largeIcons;
    }

private:
    int m_leftClick;
    int m_middleClick;
    int m_wheel;
    QSize m_thumbSize;
    ZoomFilter m_zoomFilter;
    bool m_rightToLeft;
    bool m_calculateAverageColor;
    bool m_hardwareAcceleration;
    bool m_largeIcons;
};

#endif // SETTINGSDATA_H
