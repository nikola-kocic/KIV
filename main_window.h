#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "view_files.h"
#include "pixmap_loader.h"
#include "pictureitem.h"
#include "archive_model.h"
#include "view_archive_dirs.h"

#include <QtGui/qfilesystemmodel.h>
#include <QtGui/qlineedit.h>
#include <QtGui/qsplitter.h>
#include <QtGui/qmenubar.h>
#include <QtGui/qtoolbar.h>
#include <QtGui/qcombobox.h>
#include <QtGui/qtreeview.h>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QStringList args, QWidget * parent = 0, Qt::WindowFlags f = 0 );

private slots:
    void open();
    bool saveAs();
//    void print();
//    void about();

    void rotateLeft();
    void rotateRight();
    void rotateReset();
    void zoomReset();
    void lockZoom();
    void lockAutofit();
    void lockFitWidth();
    void lockFitHeight();
    void lockNone();

    void toggleFullscreen(bool);
    void togglePanel(bool);
    void toggleLargeIcons(bool);
    void settingsDialog();
    void toggleShowThumbnails(bool);

    void refreshPath();
    void dirUp();
    void OnPathEdited();
    void OnZoomChanged();
    void OnComboBoxZoomIndexChanged(const int &index);
    void OnComboBoxZoomTextChanged();
    void OnTreeViewItemActivated ( const QModelIndex & index );

    void OnTreeViewCurrentChanged(const QModelIndex & current, const QModelIndex & previous);
    void OnFilesViewItemActivated ( const QModelIndex & index );

    void updateActions();
//    void OnPixmapLoaderFinished(QPixmap);
//    void OnImageItemActivated(QListWidgetItem*);

private:
    void createActions();
    void createMenus(QMenuBar* parent);
    void connectActions();
    bool acceptFileDrop(const QMimeData* mimeData);
    bool checkFileExtension(const QFileInfo &fi);
    void openFile(const QString &source);
    void loadImageFromIndex(const QModelIndex & index);
    void updatePath(const QString &filePath);
    bool parseZoom(const QString &zoomText);
    QString getCurrentPath();
    bool thumbs;

    QTreeView *filesystemView;
    ViewArchiveDirs *archiveDirsView;
    ViewFiles *filesView;

    QSplitter *splitterMain;
    PictureItem *imageDisplay;
    QSplitter *splitterPanel;
    QLineEdit* lineEditPath;
    QFileSystemModel *fsmTree;
    QComboBox *comboBoxZoom;
    QToolBar *toolbar;
    QToolBar *toolbarFiles;
    ArchiveModel *am;

    QAction *lineEditPathAction;

    QAction *openAct;
    QAction *saveAct;
    QAction *printAct;
    QAction *exitAct;

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

    QAction *pagePreviousAct;
    QAction *pageNextAct;
    QAction *toggleFullscreenAct;
    QAction *showThumbnailsAct;
    QAction *togglePanelAct;
    QAction *largeIconsAct;
    QAction *settingsAct;


    QAction *dirUpAct;
    QAction *refreshPathAct;

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *);
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void dropEvent(QDropEvent* event);
};

const int LV_COLNAME = 0;

#endif // MAINWINDOW_H
