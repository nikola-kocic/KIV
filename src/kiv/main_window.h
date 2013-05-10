#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "view_files.h"
#include "picture_loader.h"
#include "pictureitem.h"

#include <QSplitter>
#include <QMenuBar>
#include <QToolBar>
#include <QComboBox>
#include <QMainWindow>
#include <QLineEdit>

#include "widgets/zoomwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~MainWindow();

private:
    void createActions();
    void createMenus();
    void connectActions();
    void updateSettings();
    void populateBookmarks();
    void openFile(const FileInfo &info);
    inline void openFile(const QString &path) { openFile(FileInfo(path)); }


    Settings *m_settings;

    ViewFiles *m_view_files;

    QSplitter *m_splitter_main;
    PictureItem *m_picture_item;

    QMenuBar *m_menu_main;

    QToolBar *m_toolbar;
    QLineEdit *m_lineEdit_path;
    ZoomWidget *m_comboBox_zoom;

    /* File Menu Actions */
    QAction *m_act_open;
    QAction *m_act_save;
    QAction *m_act_exit;

    /* Bookmarks Menu Actions */
    QAction *m_act_bookmark_add;


    /* Edit Menu Actions */
    QAction *m_act_zoomIn;
    QAction *m_act_zoomOut;
    QAction *m_act_zoomReset;
    QAction *m_act_rotateLeft;
    QAction *m_act_rotateRight;
    QAction *m_act_rotateReset;
    QAction *m_act_fitToWindow;
    QAction *m_act_fitToWidth;
    QAction *m_act_fitToHeight;
    QAction *m_act_lockZoom;
    QAction *m_act_lockAutofit;
    QAction *m_act_lockFitWidth;
    QAction *m_act_lockFitHeight;
    QAction *m_act_lockNone;

    /* Options Menu Actions */
    QAction *m_act_pagePrevious;
    QAction *m_act_pageNext;
    QAction *m_act_fullscreen;
    QAction *m_act_thumbnails;
    QAction *m_act_sidebar;
    QAction *m_act_largeIcons;
    QAction *m_act_settings;


    QAction *m_act_mode_icons;
    QAction *m_act_mode_details;
    QAction *m_act_mode_list;

    /* Help Actions */
    QAction *m_act_webSite;
    QAction *m_act_about;

    /* Toolbar Actions */
    QAction *m_act_dirUp;
    QAction *m_act_refreshPath;

    QMenu *m_menu_bookmarks;
    QMenu *m_menu_context_picture;
    QMenu *m_menu_context_bookmark;

    QAction *m_act_bookmark_delete;

private slots:

    void updateActions();

    /* File Menu Actions Slots */
    void open();
    void saveAs();

    /* Bookmark Menu Action Slots */
    void addBookmark();
    void on_bookmark_triggered();
    void deleteBookmark();

    /* Edit Menu Actions Slots */
    void rotateLeft();
    void rotateRight();
    void rotateReset();
    void zoomReset();
    void lockZoom();
    void lockAutofit();
    void lockFitWidth();
    void lockFitHeight();
    void lockNone();

    /* Options Menu Actions Slots */
    void toggleFullscreen(bool);
    void toggleSidebar(bool);
    void toggleLargeIcons(bool b);
    void settingsDialog();
    void toggleShowThumbnails(bool);

    /* Help Menu Actions Slots */
    void about();
    void website();

    /* Directory Toolbar Actions Slots */
    void refreshPath();

    void on_lineEditPath_editingFinished();
    void on_lineEditPath_focus_lost();
    void on_comboBoxZoom_activated(const int &index);
    void on_filesView_currentChanged(const FileInfo &info);
    void on_customContextMenuRequested(const QPoint &pos);
    void on_bookmark_customContextMenuRequested(const QPoint &pos);
    void on_view_mode_list_triggered();
    void on_view_mode_details_triggered();
    void on_view_mode_icons_triggered();

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    bool eventFilter(QObject *, QEvent *);
};

#endif // MAINWINDOW_H
