#ifndef TEXIMG_H
#define TEXIMG_H

#include "tiledim.h"
#include "picture_loader.h"

#include <QtCore/qstring.h>
#include <QtGui/qimage.h>
#include <QtOpenGL/qgl.h>
#include <QtCore/QTime>

class TexImg : public QObject
{
    Q_OBJECT

public:
    TexImg();

    int getTexMaxSize();
    void setTexMaxSize(int size);
    void UnloadPow2Bitmap();
    void setCurrentFileInfo(const FileInfo &info);
    static const int MinTileSize = 128;
    void clearTextureCache();

    bool hasPow2Bitmap();
    uint getKbSizeTileBuffer();

    TileDim *hTile;
    TileDim *vTile;

    //[Vert][Horiz]
    QVector < QVector < GLubyte* > > pow2TileBuffer;
    int channels;

public slots:
    void CreatePow2Bitmap();

private:
    static const int TexMinSize = 16;
    static const int ThresholdTileSize = 1024;
    int texMaxSize;
    FileInfo currentFileInfo;

    void ComputeBitmapPow2Size(TileDim *tileDim);
    void InitTiles(TileDim *tileDim);
    int Pad4(int yBytes);

signals:
    void finished();

};

#endif // TEXIMG_H
