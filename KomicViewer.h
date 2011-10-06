#ifndef KOMICVIEWER_H
#define KOMICVIEWER_H

#include "generatethumbnail.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

#include "pictureitem.h"

#include <QtGui/qtreewidget.h>
#include <QtGui/qfilesystemmodel.h>
#include <QtGui/qlineedit.h>
#include <QtGui/qsplitter.h>
#include <QtGui/qmenubar.h>
#include <QtGui/qtoolbar.h>
#include <QtGui/qcombobox.h>
#include <QtGui/qlistwidget.h>

class KomicViewer : public QWidget
{
    Q_OBJECT

public:
    KomicViewer(QStringList args, QWidget * parent = 0, Qt::WindowFlags f = 0 );
    //KomicViewer();

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

    void pageNext();
    void pagePrevious();
    void toggleFullscreen(bool);
    void togglePanel(bool);
    void toggleLargeIcons(bool);
    void settingsDialog();
    void toggleShowThumbnails(bool);
    void onThumbnailFinished(IconInfo);

    void refreshPath();
    void dirUp();
    void OnPathEdited();
    void OnZoomChanged();
    void OnComboBoxZoomIndexChanged(const int &index);
    void OnComboBoxZoomTextChanged();
    void OnTreeViewCurrentChanged(const QModelIndex & current, const QModelIndex & previous);
    void OnTreeViewItemActivated ( const QModelIndex & index );
    void OnTreeFileWidgetCurrentChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void OnTreeFileWidgetItemActivated (QTreeWidgetItem * item, int column);
    void onPixmalLoaderFinished(QPixmap);
    void onThreadThumbsFinished();

private:
    void createActions();
    void createMenus(QMenuBar* parent);
    void connectActions();
    bool acceptFileDrop(const QMimeData* mimeData);
    bool checkFileExtension(const QFileInfo &fi);
    void openFile(const QString &source);
    void updatePath(const QString &filePath);
    bool parseZoom(const QString &zoomText);
    void showThumbnails();
    bool thumbs;
    int thumbCount;
    QTreeWidgetItem* AddNode(QTreeWidgetItem* node, QString name, int index);

    QList<QuaZipFileInfo> archive_files;
    QStringList filters_image;
    QStringList filters_archive;
    QSplitter *splitterMain;
    PictureItem *imageDisplay;
    QSplitter *splitterPanel;
    QLineEdit* lineEditPath;
    QTreeView *treeViewFilesystem;
    QTreeWidget *treeWidgetFiles;
    QFileSystemModel *fsm;
    QComboBox *comboBoxZoom;
    QToolBar *toolbar;
    QToolBar *toolbarFiles;
    QThread *threadThumbnails;
    QThread *threadImage;

    QListWidget *listThumbnails;

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

const int TYPE_FILE = 1001;
const int TYPE_DIR = 1002;
const int TYPE_ARCHIVE = 1003;

int makeArchiveNumberForTreewidget(int number);
int getArchiveNumberFromTreewidget(int number);

#endif // KOMICVIEWER_H
