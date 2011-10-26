#include "teximg.h"

#include <QtCore/qdebug.h>
#include <QtCore/qsize.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qimagereader.h>

TexImg::TexImg()
{
    this->channels = 4;
    this->texMaxSize = 2048;
    this->hTile = 0;
    this->vTile = 0;
}

int TexImg::getTexMaxSize()
{
    return this->texMaxSize;
}

void TexImg::setTexMaxSize(int size)
{
    this->texMaxSize = qMin(8192, size);
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
void TexImg::UnloadPow2Bitmap()
{
    if (this->hTile != 0)
    {
        delete this->hTile;
        delete this->vTile;
    }
}

int TexImg::Pad4(int yBytes)
{
    int num = yBytes % 4;
    return (num == 0) ? yBytes : (yBytes + (4 - num));
}

void TexImg::setImage(QImage img)
{
    this->UnloadPow2Bitmap();

    bitmapData = img.convertToFormat(QImage::Format_RGB32);
    this->channels = 4;
    this->hTile = new TileDim();
    this->vTile = new TileDim();
    QSize bmpSize = bitmapData.size();
    this->hTile->bmpSize = bmpSize.width();
    this->vTile->bmpSize = bmpSize.height();

    this->ComputeBitmapPow2Size(this->hTile);
    this->ComputeBitmapPow2Size(this->vTile);
    this->InitTiles(this->hTile);
    this->InitTiles(this->vTile);
}

QImage TexImg::CreatePow2Bitmap(TexIndex index)
{
    int channels = 4;
    QSize bmpSize = index.bitmapData.size();
    QImage texImage = QImage(index.currentTileWidth, index.currentTileHeight, QImage::Format_RGB32);

    int vLimit;
    if (index.vBorderOffset + index.currentTileHeight >= bmpSize.height())
    {
        vLimit = bmpSize.height() - index.vBorderOffset;
    }
    else
    {
        vLimit = index.currentTileHeight;
    }

    for (int h = 0; h < vLimit; ++h)
    {
        uchar *texPointer = texImage.scanLine(h);
        const uchar* rgb = index.bitmapData.constScanLine(index.vBorderOffset + h);
        for (int i = 0; i < index.hBorderOffset * channels; i++)
        {
            ++rgb;
        }

        int hLimit;
        if (index.hBorderOffset + index.currentTileWidth >= bmpSize.width())
        {
            hLimit = bmpSize.width() - index.hBorderOffset;
        }
        else
        {
            hLimit = index.currentTileWidth;
        }

        for (int w = 0; w < hLimit * channels; ++w)
        {
            *texPointer++ = *rgb;
            ++rgb;
        }
    }

    return texImage;
}
