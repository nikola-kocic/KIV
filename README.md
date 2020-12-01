# KIV

Cross-platform image viewer with support for images in archives


![Screenshot of KIV](https://raw.githubusercontent.com/nikola-kocic/KIV/master/kiv/assets/screenshot.jpg)


## Dependencies

### Build on Linux (qmake)

* Install quazip then build KIV by specifying quazip lib name and include path as in example:
```
mkdir build
cd build
qmake QUAZIP_LIB=-lquazip1-qt5 QUAZIP_INCLUDE="/usr/include/QuaZip-Qt5-1.1" ../kiv.pro
make
```

### Build on Linux (CMake)

* Install quazip, then build KIV with
```
mkdir build
cd build
cmake ..
make
```

### Build on Windows (MinGW)

* Install CMake (add to path)
* Make "C:\Program Files\CMake\share\cmake-3.6\Modules" folder writable by user

#### zlib
```
cd D:\Downloads\zlib128
mingw32-make -f win32/Makefile.gcc
```

#### QuaZip
```
cd D:\Downloads\quazip-0.7.2\
mkdir build
cd build
set CXXFLAGS=-std=c++11
cmake -G "MinGW Makefiles" -D CMAKE_CXX_COMPILER=g++.exe -D CMAKE_MAKE_PROGRAM=mingw32-make.exe -D ZLIB_LIBRARY=D:/Downloads/zlib128/libz.dll.a -D ZLIB_INCLUDE_DIR=D:/Downloads/zlib128 -DCMAKE_INSTALL_PREFIX:PATH=C:/QuaZip-MinGW -DBUILD_WITH_QT4:BOOL=OFF ..
mingw32-make -j4
```
Open new terminal as Admin
```
cd D:\Downloads\quazip-0.7.2\build
mingw32-make install
```


### Build on Windows (Visual Studio 2015, x64)

* Install CMake (add to path)
* Make "C:\Program Files\CMake\share\cmake-3.6\Modules" folder writable by user

#### zlib
* Open "D:\Downloads\zlib128\contrib\vstudio\vc11\zlibvc.sln"
* Change target to x64
* In "zlibvc.def", change VERSION to "1.28"
* Build zlibvc project

#### QuaZip
```
cd D:\Downloads\quazip-0.7.2\
mkdir build
cd build
cmake -G "Visual Studio 14 2015 Win64" -D ZLIB_INCLUDE_DIRS=D:/Downloads/zlib128 -DCMAKE_INSTALL_PREFIX:PATH=C:/QuaZip-VS2015-x64 -DBUILD_WITH_QT4:BOOL=OFF ..
```
* Open QuaZip.sln
* Open Properties for quazip5 project
* Open Linker->Input->Additional Dependencies
* Add D:\Downloads\zlib128\contrib\vstudio\vc11\x64\ZlibDllRelease\zlibwapi.lib
* Build quazip5 project
* Build INSTALL project
