#ifndef TEXIMG_H
#define TEXIMG_H

#include "tiledim.h"
#include "picture_loader.h"

#include <QImage>
#include <QPainter>

struct TexIndex
{
    QImage bitmapData;
    int currentTileHeight;
    int vBorderOffset;
    int currentTileWidth;
    int hBorderOffset;
    QColor background;
};

class TexImg
{
public:
    TexImg();

    int getTexMaxSize();
    void setTexMaxSize(int size);
    void setImage(QImage img);
    static const int MinTileSize = 128;

    TileDim *hTile;
    TileDim *vTile;

    static QImage CreatePow2Bitmap(TexIndex index);

private:
    static const int TexMinSize = 16;
    static const int ThresholdTileSize = 1024;
    int m_texMaxSize;

    void ComputeBitmapPow2Size(TileDim *tileDim);
    void InitTiles(TileDim *tileDim);
    int Pad4(int yBytes);

};

bool ClipTextureVertex(double texCrd1, double texCrd2, double vertexCrd1, double vertexCrd2, double texBorder1, double texBorder2, double texOffsetMin, double texScale,
                       double &texClip1, double &texClip2, double &vertexClip1, double &vertexClip2);

#endif // TEXIMG_H
