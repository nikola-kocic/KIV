#include "teximg.h"

//#include <QDebug>
#include <QSize>

TexImg::TexImg()
{
    this->m_texMaxSize = 2048;
    this->hTile = 0;
    this->vTile = 0;
}

int TexImg::getTexMaxSize()
{
    return this->m_texMaxSize;
}

void TexImg::setTexMaxSize(int size)
{
    this->m_texMaxSize = qMin(8192, size);
}

void TexImg::ComputeBitmapPow2Size(TileDim *tileDim)
{
    tileDim->pow2Size = 0;
    if (tileDim->bmpSize <= this->getTexMaxSize())
    {
        int i = TexImg::TexMinSize;
        while (i <= this->getTexMaxSize())
        {
            if (tileDim->bmpSize <= i)
            {
                if (i <= TexImg::ThresholdTileSize || tileDim->bmpSize - (i >> 1) > i >> 2)
                {
                    tileDim->pow2LastSize = (tileDim->pow2BaseSize = i);
                    tileDim->pow2BaseCount = 0;
                    tileDim->doTiling = false;
                    break;
                }
                tileDim->pow2BaseSize = i >> 1;
                tileDim->doTiling = true;
                break;
            }
            else
            {
                i <<= 1;
            }
        }
    }
    else
    {
        tileDim->pow2BaseSize = getTexMaxSize();
        tileDim->doTiling = true;
    }

    if (tileDim->doTiling)
    {
        int minTileSize = TexImg::MinTileSize;
        int num = tileDim->pow2BaseSize - minTileSize;
        tileDim->pow2BaseCount = tileDim->bmpSize / num;
        int num2 = tileDim->bmpSize % num;
        if (num2 == 0)
        {
            tileDim->pow2BaseCount--;
            tileDim->pow2LastSize = num;
        }
        else
        {
            for (int j = minTileSize; j <= tileDim->pow2BaseSize; j <<= 1)
            {
                if (num2 <= j)
                {
                    tileDim->pow2LastSize = j;
                    break;
                }
            }
        }
    }
    tileDim->pow2Size = tileDim->pow2BaseCount * tileDim->pow2BaseSize + tileDim->pow2LastSize;
}

void TexImg::InitTiles(TileDim *tileDim)
{
    int minTileSize = TexImg::MinTileSize;
    int num = tileDim->pow2BaseSize - minTileSize;
    tileDim->tileCount = tileDim->pow2BaseCount + 1;
    tileDim->tileSize = QVector<int>(tileDim->tileCount);
    tileDim->offsetBorder = QVector<int>(tileDim->tileCount + 1);
    tileDim->switchBorder = QVector<int>(tileDim->tileCount + 1);
    tileDim->offsetBorderNorm = QVector<double>(tileDim->tileCount + 1);
    tileDim->switchBorderNorm = QVector<double>(tileDim->tileCount + 1);
    tileDim->offsetBorder[0] = 0;
    tileDim->switchBorder[0] = 0;
    tileDim->tileSize[0] = tileDim->pow2BaseSize;
    for (int i = 1; i < tileDim->tileCount; ++i)
    {
        tileDim->tileSize[i] = tileDim->pow2BaseSize;
        tileDim->offsetBorder[i] = i * num;
        tileDim->switchBorder[i] = i * num + (minTileSize >> 1);
    }
    int pow2BaseCount = tileDim->pow2BaseCount;
    tileDim->tileSize[pow2BaseCount] = tileDim->pow2LastSize;
    tileDim->switchBorder[pow2BaseCount] = qMin(tileDim->bmpSize, tileDim->switchBorder.at(pow2BaseCount));
    tileDim->offsetBorder[tileDim->tileCount] = tileDim->bmpSize;
    tileDim->switchBorder[tileDim->tileCount] = tileDim->bmpSize;
    for (int j = 0; j <= tileDim->tileCount; ++j)
    {
        tileDim->offsetBorderNorm[j] = (double)tileDim->offsetBorder.at(j) / (double)tileDim->bmpSize;
        tileDim->switchBorderNorm[j] = (double)tileDim->switchBorder.at(j) / (double)tileDim->bmpSize;
    }
}

int TexImg::Pad4(int yBytes)
{
    int num = yBytes % 4;
    return (num == 0) ? yBytes : (yBytes + (4 - num));
}

void TexImg::setImage(QImage img)
{
    if (this->hTile != 0)
    {
        delete this->hTile;
        delete this->vTile;
    }
    this->hTile = new TileDim();
    this->vTile = new TileDim();
    QSize bmpSize = img.size();
    this->hTile->bmpSize = bmpSize.width();
    this->vTile->bmpSize = bmpSize.height();

    this->ComputeBitmapPow2Size(this->hTile);
    this->ComputeBitmapPow2Size(this->vTile);
    this->InitTiles(this->hTile);
    this->InitTiles(this->vTile);
}

QImage TexImg::CreatePow2Bitmap(TexIndex index)
{
    QImage texImage = QImage(index.currentTileWidth, index.currentTileHeight, QImage::Format_RGB32);

    int vLimit;
    if (index.vBorderOffset + index.currentTileHeight >= index.bitmapData.height())
    {
        vLimit = index.bitmapData.height() - index.vBorderOffset;
    }
    else
    {
        vLimit = index.currentTileHeight;
    }

    int hLimit;
    if (index.hBorderOffset + index.currentTileWidth >= index.bitmapData.width())
    {
        hLimit = index.bitmapData.width() - index.hBorderOffset;
    }
    else
    {
        hLimit = index.currentTileWidth;
    }

    QPainter p(&texImage);
    QBrush b(index.background);
    p.fillRect(texImage.rect(), b);
    p.drawImage(QRectF(0, 0, hLimit, vLimit), index.bitmapData, QRectF(index.hBorderOffset, index.vBorderOffset, hLimit, vLimit));
    p.end();

    return texImage;
}

bool ClipTextureVertex(double texCrd1, double texCrd2, double vertexCrd1, double vertexCrd2, double texBorder1, double texBorder2, double texOffsetMin, double texScale,
                       double &texClip1, double &texClip2, double &vertexClip1, double &vertexClip2)
{
    bool flag = false;
    if (texCrd1 > texCrd2)
    {
        flag = true;
        double num = texCrd1;
        texCrd1 = texCrd2;
        texCrd2 = num;
        num = vertexCrd1;
        vertexCrd1 = vertexCrd2;
        vertexCrd2 = num;
    }
    double coord1 = qMax(texCrd1, texBorder1);
    double coord2 = qMin(texCrd2, texBorder2);
    bool result = coord1 < coord2;
    double num4 = 1.0 / (texCrd2 - texCrd1);
    double num5 = (coord1 - texCrd1) * num4;
    double num6 = (coord2 - texCrd1) * num4;
    double num7 = vertexCrd2 - vertexCrd1;
    vertexClip1 = vertexCrd1 + num5 * num7;
    vertexClip2 = vertexCrd2 - (1.0 - num6) * num7;
    texClip1 = (coord1 - texOffsetMin) * texScale;
    texClip2 = (coord2 - texOffsetMin) * texScale;
    if (flag)
    {
        double num = texClip1;
        texClip1 = texClip2;
        texClip2 = num;
        num = vertexClip1;
        vertexClip1 = vertexClip2;
        vertexClip2 = num;
    }
    return result;
}
