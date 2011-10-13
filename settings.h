#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/qsettings.h>
#include <QtCore/qhash.h>
#include <QtCore/qstringlist.h>

namespace MiddleClick
{
    enum Action { None, Fullscreen, AutoFit, ZoomReset, FollowMouse, NextPage };
}

namespace Wheel
{
    enum Action { None, Scroll, ChangePage, Zoom };
}

class Settings{

public:
  static Settings  *Instance();

  MiddleClick::Action getMiddleClick();
  void setMiddleClick(MiddleClick::Action m);

  Wheel::Action getWheel();
  void setWheel(Wheel::Action w);

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

private:

// Make your constructors private
        Settings();
        Settings(const Settings&);
        Settings& operator= (const Settings&);

        QSettings *settings;

        QHash<QString, MiddleClick::Action> middleClickHash;
        QHash<QString, Wheel::Action> wheelHash;
        MiddleClick::Action getMiddleClickFromString(QString s);
        QString getStringFromMiddleClick(MiddleClick::Action a);
        Wheel::Action getWheelFromString(QString s);
        QString getStringFromWheel(Wheel::Action w);

        MiddleClick::Action middleClick;
        Wheel::Action wheel;

        bool LargeIcons;
        QString LastPath;
        QStringList filters_image;
        QStringList filters_archive;
        int thumbSize;

        bool ScrollPageByWidth;
        bool RightToLeft;
        int PageChangeTimeout;
        bool JumpToEnd;
        bool HardwareAcceleration;
};


#endif // SETTINGS_H
