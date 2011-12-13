#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/qsettings.h>
#include <QtCore/qhash.h>
#include <QtCore/qstringlist.h>

namespace MiddleClick
{
    enum Action {
        None = 0,
        Fullscreen = 1,
        AutoFit = 2,
        ZoomReset = 3,
        NextPage = 4
//        ,FollowMouse
    };
}

namespace Wheel
{
    enum Action {
        None,
        Scroll,
        ChangePage,
        Zoom
    };
}

class Settings{

public:
  static Settings  *Instance();

  MiddleClick::Action getMiddleClick();
  void setMiddleClick(int v);

  Wheel::Action getWheel();
  void setWheel(int v);

  bool getScrollPageByWidth();
  void setScrollPageByWidth(bool b);

  bool getRightToLeft();
  void setRightToLeft(bool b);

  int getPageChangeTimeout();
  void setPageChangeTimeout(int v);

  bool getLargeIcons();
  void setLargeIcons(bool b);

  QString getLastPath();
  void setLastPath(QString s);

  bool getJumpToEnd();
  void setJumpToEnd(bool b);

  bool getHardwareAcceleration();
  void setHardwareAcceleration(bool b);

  QStringList getFiltersImage();
  QStringList getFiltersArchive();

  int getThumbnailSize();
  void setThumbnailSize(int v);

  bool getScrollChangesPage();
  void setScrollChangesPage(bool b);


private:

/* Make your constructors private */
        Settings();
        Settings(const Settings&);
        Settings& operator= (const Settings&);

        QSettings *settings;

        MiddleClick::Action middleClick;
        Wheel::Action wheel;

        bool largeIcons;
        QString LastPath;
        QStringList filters_image;
        QStringList filters_archive;

        bool scrollPageByWidth;
        bool rightToLeft;
        int pageChangeTimeout;
        bool jumpToEnd;
        bool scrollChangesPage;

        bool HardwareAcceleration;
        int thumbSize;
};


#endif // SETTINGS_H
