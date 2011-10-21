#ifndef TILEDIM_H
#define TILEDIM_H

#include <QtCore/qvector.h>

class TileDim
{
public:
    TileDim() {};

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

#endif // TILEDIM_H
