#ifndef PICTUREITEM_INTERFACE_H
#define PICTUREITEM_INTERFACE_H

#include "widgets/picture_item/pictureitem_data.h"
#include "fileinfo.h"
#include <QImage>

class PictureItemInterface
{
public:
    explicit PictureItemInterface(PictureItemData *data)
        : m_data(data), m_widget(nullptr)
    {}
    virtual ~PictureItemInterface() {}
    virtual void setRotation(const qreal current, const qreal previous) = 0;
    virtual void setZoom(const qreal current, const qreal previous) = 0;
    virtual void setImage(const QImage &img) = 0;

    virtual void setNullImage()
    { this->setImage(QImage()); }

    virtual void setBackgroundColor(const QColor &color)
    { m_data->m_color_clear = color; }

    QWidget* getWidget();

protected:
    PictureItemData *m_data;
    QWidget* m_widget;
};

inline QWidget* PictureItemInterface::getWidget()
{ return m_widget; }

#endif  // PICTUREITEM_INTERFACE_H
