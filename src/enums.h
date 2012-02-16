#ifndef ENUMS_H
#define ENUMS_H

struct LockMode
{
    static const int None = 0;
    static const int Autofit = 1;
    static const int FitWidth = 2;
    static const int FitHeight = 3;
    static const int Zoom = 4;
};

struct MiddleClickAction
{
    static const int None = 0;
    static const int Fullscreen = 1;
    static const int AutoFit = 2;
    static const int ZoomReset = 3;
    static const int NextPage = 4;
    static const int Quit = 5;
    static const int Boss = 6;
};

struct WheelAction
{
    static const int None = 0;
    static const int Scroll = 1;
    static const int ChangePage = 2;
    static const int Zoom = 3;
};

struct FileViewMode
{
    static const int List = 0;
    static const int Details = 1;
    static const int Icons = 2;
};

struct SortDirection
{
    static const int NameAsc = 0;
    static const int NameDesc = 1;
    static const int DateAsc = 2;
    static const int DateDesc = 3;
    static const int SizeAsc = 4;
    static const int SizeDesc = 5;
};

#endif // ENUMS_H
