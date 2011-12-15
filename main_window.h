#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "view_files.h"
#include "picture_loader.h"
#include "pictureitem.h"
#include "model_files.h"
#include "view_archive_dirs.h"

#include <QFileSystemModel>
#include <QSplitter>
#include <QMenuBar>
#include <QToolBar>
#include <QComboBox>
#include <QTreeView>
#include <QMainWindow>
#include <QLineEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);

private:
    void createActions();
    void createMenus();
    void connectActions();
    void populateBookmarks();
    bool acceptFileDrop(const QMimeData *mimeData);
    void openFile(const QString &source);

    Settings *m_settings;
    QFileSystemModel *m_model_filesystem;
    FilesModel *m_model_files;

    QTreeView *m_view_filesystem;
    ViewArchiveDirs *m_view_archiveDirs;
    ViewFiles *m_view_files;

    QSplitter *m_splitter_main;
    QSplitter *m_splitter_sidebar;
    PictureItem *m_picture_item;

    QMenuBar *m_menu_main;

    QToolBar *m_toolbar;
    QLineEdit *m_lineEdit_path;
    QComboBox *m_comboBox_zoom;

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
    bool saveAs();

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
    void toggleLargeIcons(bool);
    void settingsDialog();
    void toggleShowThumbnails(bool);

    /* Help Menu Actions Slots */
    void about();
    void website();

    /* Directory Toolbar Actions Slots */
    void refreshPath();
    void dirUp();

    void on_lineEditPath_editingFinished();
    void on_lineEditPath_focus_lost();
    void on_zoom_changed(qreal current, qreal previous);
    void on_comboBoxZoom_activated(const int &index);
    void on_comboBoxZoom_TextChanged();
    void on_comboBoxZoom_focus_lost();
    void on_filesystemView_currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_filesView_item_activated(const QModelIndex &index);
    void on_filesView_currentChanged(const FileInfo &info);
    void on_customContextMenuRequested(const QPoint &pos);
    void on_bookmark_customContextMenuRequested(const QPoint &pos);

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    bool eventFilter(QObject *, QEvent *);
};

const int LV_COLNAME = 0;

#endif // MAINWINDOW_H
