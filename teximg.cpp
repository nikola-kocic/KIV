#include "teximg.h"

#include <QtCore/qdebug.h>
#include <QtCore/qsize.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qimagereader.h>

TexImg::TexImg()
{
    this->channels = 4;
    this->texMaxSize = 2048;
}

bool TexImg::hasPow2Bitmap()
{
    return !(this->pow2TileBuffer.isEmpty());
}

int TexImg::getTexMaxSize()
{
    return this->texMaxSize;
}

void TexImg::setTexMaxSize(int size)
{
    //was 8192
    this->texMaxSize = qMin(2048, size);
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
//    qDebug("InitTiles %x", tileDim);
}
void TexImg::UnloadPow2Bitmap()
{
    if (this->hasPow2Bitmap())
    {
        delete this->hTile;
        delete this->vTile;
    }
}

void TexImg::clearTextureCache()
{
    //Clear textures from memory
    for (int hIndex = 0; hIndex < this->pow2TileBuffer.count(); ++hIndex)
    {
        for (int vIndex = 0; vIndex < this->pow2TileBuffer.at(hIndex).count(); ++vIndex)
        {
            delete this->pow2TileBuffer.at(hIndex).at(vIndex);
        }
    }
    this->pow2TileBuffer.clear();
}

int TexImg::Pad4(int yBytes)
{
    int num = yBytes % 4;
    return (num == 0) ? yBytes : (yBytes + (4 - num));
}

void TexImg::setCurrentFileInfo(const FileInfo &info)
{
    this->currentFileInfo = info;
}

void TexImg::CreatePow2Bitmap()
{
    this->clearTextureCache();
    this->UnloadPow2Bitmap();

    if (this->hasPow2Bitmap())
    {
        return;
    }

    QTime t;
    t.start();

    qDebug() << "CreatePow2Bitmap - start" << t.elapsed();
    QImage bitmapData = PictureLoader::getImage(this->currentFileInfo);
    qDebug() << "CreatePow2Bitmap - bitmapData" << bitmapData.size() << t.elapsed();
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

    qDebug() << "CreatePow2Bitmap - inittiles" << t.elapsed();

//    qDebug() << "hTile" << "\nbmpSize" << hTile->bmpSize << "\ndoTiling" << hTile->doTiling << "\noffsetBorder" << hTile->offsetBorder
//             << "\noffsetBorderNorm" << hTile->offsetBorderNorm << "\npow2BaseCount" << hTile->pow2BaseCount << "\npow2BaseSize" << hTile->pow2BaseSize
//             << "\npow2LastSize" << hTile->pow2LastSize << "\npow2Size" << hTile->pow2Size << "\nswitchBorder" << hTile->switchBorder << "\nswitchBorderNorm" << hTile->switchBorderNorm
//             << "\ntileCount" << hTile->tileCount << "\ntileSize" << hTile->tileSize;

//    qDebug() << "vTile" << "\nbmpSize" << vTile->bmpSize << "\ndoTiling" << vTile->doTiling << "\noffsetBorder" << vTile->offsetBorder
//             << "\noffsetBorderNorm" << vTile->offsetBorderNorm << "\npow2BaseCount" << vTile->pow2BaseCount << "\npow2BaseSize" << vTile->pow2BaseSize
//             << "\npow2LastSize" << vTile->pow2LastSize << "\npow2Size" << vTile->pow2Size << "\nswitchBorder" << vTile->switchBorder << "\nswitchBorderNorm" << vTile->switchBorderNorm
//             << "\ntileCount" << vTile->tileCount << "\ntileSize" << vTile->tileSize;

    this->pow2TileBuffer = QVector < QVector < GLubyte* > >(this->hTile->tileCount);
    for (int i = 0; i < this->hTile->tileCount; ++i)
    {
        this->pow2TileBuffer[i].resize(this->vTile->tileCount);

        for (int j = 0; j < this->vTile->tileCount; ++j)
        {
            this->pow2TileBuffer[i][j] = new GLubyte[this->vTile->tileSize.at(j) * this->hTile->tileSize.at(i) * this->channels];
        }
    }

    t.restart();
    qDebug() << "CreatePow2Bitmap - pow2TileBuffer" << t.elapsed();

    qDebug() << bitmapData.format();
    bitmapData = bitmapData.convertToFormat(QImage::Format_RGB32);
    for (int vTileIndex = 0; vTileIndex < this->vTile->tileCount; ++vTileIndex)
    {
        int CurrentTileHeight = this->vTile->tileSize.at(vTileIndex);
        int vBorderOffset = this->vTile->offsetBorder.at(vTileIndex);
        for (int hTileIndex = 0; hTileIndex < this->hTile->tileCount; ++hTileIndex)
        {
            GLubyte *texImage = this->pow2TileBuffer.at(hTileIndex).at(vTileIndex);
            int CurrentTileWidth = this->hTile->tileSize.at(hTileIndex);
            int hBorderOffset = this->hTile->offsetBorder.at(hTileIndex);

            int vLimit;
            if (vBorderOffset + CurrentTileHeight >= bmpSize.height())
            {
                vLimit = bmpSize.height() - vBorderOffset;
            }
            else
            {
                vLimit = CurrentTileHeight;
            }

            for (int h = 0; h < vLimit; ++h)
            {
                GLubyte *texPointer = &(texImage[this->channels * (h * CurrentTileWidth)]);
                const QRgb* rgb = (const QRgb*)bitmapData.constScanLine(vBorderOffset + h);
                for (int i = 0; i < hBorderOffset; i++)
                {
                    ++rgb;
                }

                int hLimit;
                if (hBorderOffset + CurrentTileWidth >= bmpSize.width())
                {
                    hLimit = bmpSize.width() - hBorderOffset;
                }
                else
                {
                    hLimit = CurrentTileWidth;
                }

                for (int w = 0; w < hLimit; ++w)
                {
                    *texPointer++ = qRed(*rgb);
                    *texPointer++ = qGreen(*rgb);
                    *texPointer++ = qBlue(*rgb);
                    *texPointer++ = qAlpha(*rgb);
                    ++rgb;
                }
            }
        }
    }

    qDebug() << "CreatePow2Bitmap - end" << t.elapsed();

    emit finished();
    return;
}
