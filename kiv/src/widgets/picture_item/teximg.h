#ifndef TEXIMG_H
#define TEXIMG_H

#include <QImage>
#include <QPainter>

#include "kiv/src/picture_loader.h"

struct TexIndex
{
    QImage bitmapData;
    int currentTileHeight;
    int vBorderOffset;
    int currentTileWidth;
    int hBorderOffset;
    QColor background;
};

struct TileDim
{
    int bmpSize;
    int pow2Size;
    int pow2BaseSize;
    int pow2BaseCount;
    int pow2LastSize;
    bool doTiling;
    int tileCount;
    QVector<int> tileSize;
    QVector<int> switchBorder;
    QVector<int> offsetBorder;
    QVector<double> switchBorderNorm;
    QVector<double> offsetBorderNorm;
};

class TexImg
{
public:
    explicit TexImg();
    ~TexImg();

    int getTexMaxSize() const;
    void setTexMaxSize(const int size);
    void setImage(const QImage &img);
    static const int MinTileSize = 128;

    TileDim *hTile;
    TileDim *vTile;

    static QImage CreatePow2Bitmap(const TexIndex *const index);

private:
    static const int TexMinSize = 16;
    static const int ThresholdTileSize = 1024;
    int m_texMaxSize;

    void ComputeBitmapPow2Size(TileDim *tileDim);
    void InitTiles(TileDim *tileDim);

};

bool ClipTextureVertex(double texCrd1,
                       double texCrd2,
                       double vertexCrd1,
                       double vertexCrd2,
                       double texBorder1,
                       double texBorder2,
                       double texOffsetMin,
                       double texScale,
                       double &texClip1,
                       double &texClip2,
                       double &vertexClip1,
                       double &vertexClip2);

#endif  // TEXIMG_H
