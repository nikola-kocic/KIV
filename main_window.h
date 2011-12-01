#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "view_files.h"
#include "picture_loader.h"
#include "pictureitem.h"
#include "model_files.h"
#include "view_archive_dirs.h"

#include <QtGui/qfilesystemmodel.h>
#include <QtGui/qlineedit.h>
#include <QtGui/qsplitter.h>
#include <QtGui/qmenubar.h>
#include <QtGui/qtoolbar.h>
#include <QtGui/qcombobox.h>
#include <QtGui/qtreeview.h>
#include <QtGui/qmainwindow.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);

private:
    void createActions();
    void createMenus(QMenuBar *parent);
    void connectActions();
    bool acceptFileDrop(const QMimeData *mimeData);
    void openFile(const QString &source);
    void updatePath(const QString &filePath);

    QFileSystemModel *modelFilesystem;
    FilesModel *modelFiles;

    QTreeView *filesystemView;
    ViewArchiveDirs *archiveDirsView;
    ViewFiles *filesView;

    QSplitter *splitterMain;
    PictureItem *imageDisplay;
    QSplitter *splitterPanel;

    QMenuBar *mainMenu;

    QToolBar *toolbar;
    QLineEdit *lineEditPath;
    QComboBox *comboBoxZoom;

    /* File Menu Actions */
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;

    /* Edit Menu Actions */
    QAction *rotateLeftAct;
    QAction *rotateRightAct;
    QAction *rotateResetAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *zoomResetAct;
    QAction *fitToWindowAct;
    QAction *fitToWidthAct;
    QAction *fitToHeightAct;
    QAction *lockZoomAct;
    QAction *lockAutofitAct;
    QAction *lockFitWidthAct;
    QAction *lockFitHeightAct;
    QAction *lockNoneAct;

    /* Options Menu Actions */
    QAction *pagePreviousAct;
    QAction *pageNextAct;
    QAction *toggleFullscreenAct;
    QAction *exitFullscreenAct;
    QAction *showThumbnailsAct;
    QAction *togglePanelAct;
    QAction *largeIconsAct;
    QAction *settingsAct;

    /* Help Actions */
    QAction *webSiteAct;
    QAction *aboutAct;

    /* Toolbar Actions */
    QAction *dirUpAct;
    QAction *refreshPathAct;

    QMenu *contextMenu;

private slots:

    void updateActions();

    /* File Menu Actions Slots */
    void open();
    bool saveAs();

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
    void togglePanel(bool);
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
    void on_zoom_changed(qreal current, qreal previous);
    void on_comboBoxZoom_activated(const int &index);
    void on_comboBoxZoom_TextChanged();
    void on_filesystemView_item_clicked(const QModelIndex &index);
    void on_filesystemView_currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_filesView_item_activated(const QModelIndex &index);
    void on_filesView_currentChanged(const FileInfo &info);
    void on_customContextMenuRequested(const QPoint &pos);

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
};

const int LV_COLNAME = 0;

#endif // MAINWINDOW_H
