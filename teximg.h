#ifndef TEXIMG_H
#define TEXIMG_H

#include <QtCore/qstring.h>
#include "tiledim.h"
#include <QGLWidget>
#include <QtGui/qimage.h>

//enum EnumImageLoad
//{
//        Undefined,
//        ReloadImage,
//        PlayNextImage,
//        NextImage,
//        PreviousImage,
//        LoadFile,
//        NextFile,
//        PreviousFile
//};

class TexImg
{

public:
    TexImg();

    int getTexMaxSize();
    void setTexMaxSize(int size);
    void UnloadPow2Bitmap();
    void CreatePow2Bitmap();
    static const int MinTileSize = 128;

    bool hasPow2Bitmap();
    uint getKbSizeTileBuffer();

    TileDim *hTile;
    TileDim *vTile;
    QVector < QVector < GLubyte* > > pow2TileBuffer;
    int channels;
    TexImg *prevTexImg;
    TexImg *nextTexImg;
//    RenderImageState renderImageState = new RenderImageState();
//    long createPow2BitmapTime;
//    EnumImageLoad imageLoad;

private:
    static const int TexMinSize = 16;
    static const int ThresholdTileSize = 1024;
    int texMaxSize;

    void ComputeBitmapPow2Size(TileDim *tileDim);
    void InitTiles(TileDim *tileDim);
    int Pad4(int yBytes);

};

#endif // TEXIMG_H
