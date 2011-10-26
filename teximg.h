#ifndef TEXIMG_H
#define TEXIMG_H

#include "tiledim.h"
#include "picture_loader.h"

#include <QtCore/qstring.h>
#include <QtGui/qimage.h>
#include <QtOpenGL/qgl.h>
#include <QtCore/QTime>

struct TexIndex{
    QImage bitmapData;
    int currentTileHeight;
    int vBorderOffset;
    int currentTileWidth;
    int hBorderOffset;
};

class TexImg
{
public:
    TexImg();

    int getTexMaxSize();
    void setTexMaxSize(int size);
    void UnloadPow2Bitmap();
    void setImage(QImage img);
    static const int MinTileSize = 128;
    uint getKbSizeTileBuffer();
    QImage bitmapData;

    TileDim *hTile;
    TileDim *vTile;

    //[Vert][Horiz]
    int channels;

public slots:
    static QImage CreatePow2Bitmap(TexIndex index);

private:
    static const int TexMinSize = 16;
    static const int ThresholdTileSize = 1024;
    int texMaxSize;

    void ComputeBitmapPow2Size(TileDim *tileDim);
    void InitTiles(TileDim *tileDim);
    int Pad4(int yBytes);

};

#endif // TEXIMG_H
