#include "main_window.h"
#include "settings_dialog.h"
#include "system_icons.h"
#include "settings.h"

#include <QtCore/qdebug.h>
//#include <QtCore/qdatetime.h>
#include <QtCore/qbuffer.h>
#include <QtCore/QUrl>

//#include <QtGui/QSpacerItem>
//#include <QtGui/qmessagebox.h>
#include <QtGui/qapplication.h>
#include <QtGui/qaction.h>
#include <QtGui/qevent.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qimagereader.h>
#include <QtGui/qfiledialog.h>
#include <QtGui/qheaderview.h>
#include <QtGui/qfileiconprovider.h>
#include <QtGui/qdesktopwidget.h>
#include <QtGui/qcompleter.h>

MainWindow::MainWindow (QStringList args, QWidget * parent, Qt::WindowFlags f)
{
    threadImage = new QThread();

    resize(QApplication::desktop()->width() - 100,
		QApplication::desktop()->height() - 100);
    setWindowTitle(QApplication::applicationName() + " " + QApplication::applicationVersion());

    setWindowIcon(QIcon(":/icons/komicviewer.svg"));

    fsmTree = new QFileSystemModel(this);
    QStringList filters;

    foreach (const QString &ext, Settings::Instance()->getFiltersArchive())
    {
        filters << "*." + ext;
    }

    fsmTree->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    fsmTree->setNameFilterDisables(false);
    fsmTree->setNameFilters(filters);
    fsmTree->setRootPath("");

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    policy.setHorizontalStretch(1);
    policy.setVerticalStretch(0);

    am = new ArchiveModel();
    createActions();

    splitterMain = new QSplitter(Qt::Horizontal, this);

    QVBoxLayout *vboxMain = new QVBoxLayout(this);
    vboxMain->setSpacing(0);
    vboxMain->setMargin(0);


    QMenuBar *mainMenu = new QMenuBar();
    mainMenu->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    createMenus(mainMenu);

    lineEditPath = new QLineEdit();

    comboBoxZoom = new QComboBox();
    comboBoxZoom->setEnabled(false);
    comboBoxZoom->setInsertPolicy(QComboBox::NoInsert);
    comboBoxZoom->setMaxVisibleItems(12);
    comboBoxZoom->setMinimumContentsLength(8);
    comboBoxZoom->setEditable(true);
    comboBoxZoom->setFocusPolicy(Qt::ClickFocus);

    toolbar = new QToolBar();
    toolbar->setMovable(false);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
//    toolbar->setIconSize(QSize(16,16));
    toolbar->layout()->setMargin(0);

    toolbar->addAction(togglePanelAct);
    toolbar->addAction(toggleFullscreenAct);
    toolbar->addSeparator();
    toolbar->addWidget(mainMenu);
    toolbar->addSeparator();
    toolbar->addAction(pagePreviousAct);
    toolbar->addAction(pageNextAct);
    toolbar->addSeparator();
    toolbar->addAction(zoomInAct);
    toolbar->addAction(zoomOutAct);
//    toolbar->addAction(normalSizeAct);
    toolbar->addAction(fitToWindowAct);
    toolbar->addWidget(comboBoxZoom);
    toolbar->addSeparator();
    toolbar->addAction(rotateLeftAct);
    toolbar->addAction(rotateRightAct);


    toolbarFiles = new QToolBar();
    toolbarFiles->setMovable(false);
    toolbarFiles->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbarFiles->addAction(dirUpAct);
    toolbarFiles->addAction(refreshPathAct);
    lineEditPathAction = toolbarFiles->addWidget(lineEditPath);



    vboxMain->addWidget(toolbar);
    vboxMain->addWidget(toolbarFiles);

    //Panel start

    splitterPanel = new QSplitter(Qt::Vertical, this);

    treeViewFilesystem = new QTreeView();
    treeViewFilesystem->setUniformRowHeights(true);
    treeViewFilesystem->setHeaderHidden(true);

    splitterPanel->addWidget(treeViewFilesystem);

    QSplitter *splitterFiles = new QSplitter(Qt::Vertical, this);

    treeViewArchiveDirs = new ViewArchiveDirs();
//    treeViewArchiveDirs->header()->setResizeMode(QHeaderView::Stretch);
    treeViewArchiveDirs->setUniformRowHeights(true);
    treeViewArchiveDirs->setHeaderHidden(true);
    treeViewArchiveDirs->setEditTriggers(QAbstractItemView::NoEditTriggers);

    treeViewArchiveDirs->hide();
//    treeViewArchiveDirs->setHeaderLabels(QStringList() << "Name");

    splitterFiles->addWidget(treeViewArchiveDirs);
    splitterFiles->setSizes(QList<int>() << 300);

    fileList = new ThumbnailViewer(am);
    fileList->setSizePolicy(policy);
    splitterFiles->addWidget(fileList);

    splitterPanel->addWidget(splitterFiles);

    splitterMain->addWidget(splitterPanel);

    splitterMain->setSizes(QList<int>() << 300);

    //Panel end


    //Content start
    imageDisplay = new PictureItem(Settings::Instance()->getHardwareAcceleration());

    imageDisplay->setSizePolicy(policy);

    splitterMain->addWidget(imageDisplay);
    //Content end


    vboxMain->addWidget(splitterMain);
    setLayout(vboxMain);

    treeViewFilesystem->setModel(fsmTree);
    for(int i = 1; i < treeViewFilesystem->header()->count(); i++) treeViewFilesystem->hideColumn(i);

    treeViewArchiveDirs->setModel(am);



    foreach(const qreal &i, imageDisplay->getDefaultZoomSizes())
    {
	comboBoxZoom->addItem(QString::number((int)(i * 100)) + "%");
	if(i == 1) comboBoxZoom->setCurrentIndex(comboBoxZoom->count() - 1);
    }

    // Now add the line to the splitter handle
    // Note: index 0 handle is always hidden, index 1 is between the two widgets
    QSplitterHandle *handleMain = splitterMain->handle(1);
    QVBoxLayout *layoutMain = new QVBoxLayout(handleMain);
    layoutMain->setSpacing(0);
    layoutMain->setMargin(0);
    QFrame *frameMain = new QFrame(handleMain);
    frameMain->setFrameShape(QFrame::Panel);
    frameMain->setFrameShadow(QFrame::Raised);
    layoutMain->addWidget(frameMain);

    // Now add the line to the splitter handle
    // Note: index 0 handle is always hidden, index 1 is between the two widgets
    QSplitterHandle *handlePanel = splitterPanel->handle(1);
    QVBoxLayout *layoutPanel = new QVBoxLayout(handlePanel);
    layoutPanel->setSpacing(0);
    layoutPanel->setMargin(0);
    QFrame *framePanel = new QFrame(handlePanel);
    framePanel->setFrameShape(QFrame::Panel);
    framePanel->setFrameShadow(QFrame::Raised);
    layoutPanel->addWidget(framePanel);


    //Large icons are On by default but I want small icons by default
    if(Settings::Instance()->getLargeIcons() == true)
    {
        largeIconsAct->setChecked(true);
    }
    else
    {
        toggleLargeIcons(false);
    }

    connectActions();
    connect(fileList, SIGNAL(activated(QModelIndex)), this, SLOT(OnImageItemActivated(QModelIndex)));
    connect(lineEditPath, SIGNAL(editingFinished()), this, SLOT(OnPathEdited()));
    connect(treeViewFilesystem, SIGNAL(clicked(QModelIndex)), this, SLOT(OnTreeViewItemActivated(QModelIndex)));

    connect(treeViewFilesystem->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(OnTreeViewCurrentChanged(QModelIndex,QModelIndex)));
    connect(treeViewArchiveDirs->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), fileList, SLOT(OnTreeViewArchiveDirsCurrentChanged(QModelIndex,QModelIndex)));
    connect(fileList->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(OnFileListCurrentRowChanged(QModelIndex,QModelIndex)));


    connect(imageDisplay, SIGNAL(toggleFullscreen()), toggleFullscreenAct, SLOT(toggle()));
    connect(imageDisplay, SIGNAL(pageNext()), this, SLOT(pageNext()));
    connect(imageDisplay, SIGNAL(pagePrevious()), this, SLOT(pagePrevious()));
    connect(imageDisplay, SIGNAL(zoomChanged()), this, SLOT(OnZoomChanged()));
    connect(comboBoxZoom, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxZoomIndexChanged(int)));
    connect(comboBoxZoom->lineEdit(), SIGNAL(returnPressed()), this, SLOT(OnComboBoxZoomTextChanged()));

    connect(refreshPathAct, SIGNAL(triggered()), this, SLOT(refreshPath()));
    connect(dirUpAct, SIGNAL(triggered()), this, SLOT(dirUp()));

    pl = new PixmapLoader();
    connect(threadImage, SIGNAL(started()), pl, SLOT(loadPixmap()));
    connect(pl, SIGNAL(finished(QPixmap)), this, SLOT(OnPixmapLoaderFinished(QPixmap)));
    pl->moveToThread(threadImage);

    QCompleter *completer = new QCompleter(this);
    completer->setModel(fsmTree);
    lineEditPath->setCompleter(completer);

    if(args.count() > 1)
    {
	openFile(args[1]);
    }
    else
    {
        QString path = Settings::Instance()->getLastPath();
        if(path != "")
        {
            treeViewFilesystem->setCurrentIndex(fsmTree->index(path));
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Settings::Instance()->setLastPath(getCurrentPath());
}

QString MainWindow::getCurrentPath()
{
    return fsmTree->filePath(treeViewFilesystem->currentIndex());
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
//	qDebug() << "kv" << event->key();
	if(lineEditPath->hasFocus())
	{
//	    if(lineEditPath->palette() != QApplication::palette())
//	    {
//		lineEditPath->setPalette(QApplication::palette());
//	    }

//	    updatePath(fsm->filePath(index));

	    refreshPath();

	    event->accept();
	}
	else if(comboBoxZoom->lineEdit()->hasFocus())
	{
	    OnZoomChanged();
	    event->accept();
	}
    }
}

void MainWindow::createActions()
{
//    qDebug() << QIcon::themeSearchPaths();
//qDebug() << QCoreApplication::applicationDirPath();
    static const char * GENERIC_ICON_TO_CHECK = "media-skip-backward";
    static const char * FALLBACK_ICON_THEME = "glyphs";
    if (!QIcon::hasThemeIcon(GENERIC_ICON_TO_CHECK)) {
        //If there is no default working icon theme then we should
        //use an icon theme that we provide via a icons folder
        //This case happens under Windows and Mac OS X
        //This does not happen under GNOME or KDE
//        QIcon::setThemeSearchPaths(QStringList() << QCoreApplication::applicationDirPath());
        QIcon::setThemeName(FALLBACK_ICON_THEME);
//        qDebug() << "theme fall back";
    }

    openAct = new QAction(QIcon::fromTheme("document-open"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));

    saveAct = new QAction(QIcon::fromTheme("document-save-as"), tr("&Save Page As..."), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setEnabled(false);

    //    printAct = new QAction(tr("&Print..."), this);
    //    printAct->setShortcut(tr("Ctrl+P"));
    //    printAct->setEnabled(false);
    //    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    exitAct = new QAction(QIcon::fromTheme("application-exit"), tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));

    //Options Actions

    pagePreviousAct = new QAction(QIcon::fromTheme("media-skip-backward"), tr("&Previous Page"), this);
    pagePreviousAct->setShortcut(Qt::Key_PageUp);

    pageNextAct = new QAction(QIcon::fromTheme("media-skip-forward"), tr("&Next Page"), this);
    pageNextAct->setShortcut(Qt::Key_PageDown);

    rotateLeftAct = new QAction(QIcon::fromTheme("object-rotate-left"), tr("Rotate &Left"), this);
    rotateLeftAct->setEnabled(false);

    rotateRightAct = new QAction(QIcon::fromTheme("object-rotate-right"), tr("Rotate &Right"), this);
    rotateRightAct->setEnabled(false);

    rotateResetAct = new QAction(tr("R&eset Rotation"), this);
    rotateResetAct->setEnabled(false);

    showThumbnailsAct = new QAction(QIcon::fromTheme("view-list-icons"), tr("&Show Thumbnails"), this);
    showThumbnailsAct->setCheckable(true);

    toggleFullscreenAct = new QAction(QIcon::fromTheme("view-fullscreen"), tr("&Full Screen"), this);
    toggleFullscreenAct->setShortcut(Qt::Key_F11);
    toggleFullscreenAct->setCheckable(true);

    togglePanelAct = new QAction(QIcon::fromTheme("view-split-left-right"),tr("Show Side&bar"), this);
    togglePanelAct->setCheckable(true);
    togglePanelAct->setChecked(true);

    largeIconsAct = new QAction(tr("Large &Icons"), this);
    largeIconsAct->setCheckable(true);

    settingsAct = new QAction(QIcon::fromTheme("configure", QIcon::fromTheme("gtk-preferences")), tr("&Settings..."), this);
    settingsAct->setMenuRole(QAction::PreferencesRole);
//    settingsAct->setVisible(false);



    //Zoom Actions

    zoomInAct = new QAction(QIcon::fromTheme("zoom-in"), tr("Zoom &In"), this);
    zoomInAct->setShortcut(Qt::CTRL | Qt::Key_Plus);
    zoomInAct->setEnabled(false);

    zoomOutAct = new QAction(QIcon::fromTheme("zoom-out"), tr("Zoom &Out"), this);
    zoomOutAct->setShortcut(Qt::CTRL | Qt::Key_Minus);
    zoomOutAct->setEnabled(false);

    zoomResetAct = new QAction(QIcon::fromTheme("zoom-original"), tr("&Original Size (100%)"), this);
    zoomResetAct->setShortcut(Qt::CTRL | Qt::Key_1);
    zoomResetAct->setEnabled(false);

    fitToWindowAct = new QAction(QIcon::fromTheme("zoom-fit-best"), tr("&Fit to Window"), this);
    fitToWindowAct->setShortcut(Qt::CTRL | Qt::Key_T);
    fitToWindowAct->setEnabled(false);

    fitToWidthAct = new QAction(QIcon::fromTheme("zoom-fit-width"), tr("Fit &Width"), this);
    fitToWidthAct->setEnabled(false);

    fitToHeightAct = new QAction(QIcon::fromTheme("zoom-fit-height"), tr("Fit &Height"), this);
    fitToHeightAct->setEnabled(false);

    lockNoneAct = new QAction(tr("Lock &None"), this);
    lockNoneAct->setCheckable(true);
    lockNoneAct->setChecked(true);

    lockAutofitAct = new QAction(tr("Lock Autofi&t"), this);
    lockAutofitAct->setCheckable(true);

    lockFitHeightAct = new QAction(tr("Lock Fit Hei&ght"), this);
    lockFitHeightAct->setCheckable(true);

    lockFitWidthAct = new QAction(tr("Lock Fit Wi&dth"), this);
    lockFitWidthAct->setCheckable(true);

    lockZoomAct = new QAction(tr("Lock Zoom &Value"), this);
    lockZoomAct->setCheckable(true);

    refreshPathAct = new QAction(QIcon::fromTheme("view-refresh"), tr("&Refresh"), this);

    dirUpAct = new QAction(QIcon::fromTheme("go-up"), tr("Go &Up"), this);
    dirUpAct->setEnabled(false);
}

void MainWindow::createMenus(QMenuBar *parent)
{
    QMenu *fileMenu = new QMenu(tr("&File"), parent);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
//    fileMenu->addAction(printAct);
//    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);


    QMenu *editMenu = new QMenu(tr("&Edit"), parent);

    editMenu->addAction(rotateLeftAct);
    editMenu->addAction(rotateRightAct);
    editMenu->addAction(rotateResetAct);

    editMenu->addSeparator();

    editMenu->addAction(zoomInAct);
    this->addAction(zoomInAct);
    editMenu->addAction(zoomOutAct);
    this->addAction(zoomOutAct);
    editMenu->addAction(zoomResetAct);
    this->addAction(zoomResetAct);
    editMenu->addSeparator();
    editMenu->addAction(fitToWindowAct);
    this->addAction(fitToWindowAct);
    editMenu->addAction(fitToWidthAct);
    editMenu->addAction(fitToHeightAct);
    editMenu->addSeparator();

    QActionGroup *lockActions = new QActionGroup(this);

    lockActions->addAction(lockNoneAct);
    lockActions->addAction(lockZoomAct);
    lockActions->addAction(lockAutofitAct);
    lockActions->addAction(lockFitWidthAct);
    lockActions->addAction(lockFitHeightAct);

    editMenu->addAction(lockNoneAct);
    editMenu->addAction(lockZoomAct);
    editMenu->addAction(lockAutofitAct);
    editMenu->addAction(lockFitWidthAct);
    editMenu->addAction(lockFitHeightAct);



    QMenu *optionsMenu = new QMenu(tr("&Options"), parent);
    this->addAction(pagePreviousAct);
    this->addAction(pageNextAct);
    optionsMenu->addAction(pagePreviousAct);
    optionsMenu->addAction(pageNextAct);
    optionsMenu->addSeparator();
    optionsMenu->addAction(showThumbnailsAct);
    this->addAction(toggleFullscreenAct);
    optionsMenu->addAction(toggleFullscreenAct);
    optionsMenu->addAction(togglePanelAct);
    optionsMenu->addAction(largeIconsAct);
    optionsMenu->addAction(settingsAct);

    parent->addMenu(fileMenu);
    parent->addMenu(editMenu);
    parent->addMenu(optionsMenu);
}


void MainWindow::connectActions()
{
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    connect(settingsAct, SIGNAL(triggered()), this, SLOT(settingsDialog()));
    connect(pagePreviousAct, SIGNAL(triggered()), this, SLOT(pagePrevious()));
    connect(pageNextAct, SIGNAL(triggered()), this, SLOT(pageNext()));


    connect(showThumbnailsAct, SIGNAL(toggled(bool)), this, SLOT(toggleShowThumbnails(bool)));
    connect(toggleFullscreenAct, SIGNAL(toggled(bool)), this, SLOT(toggleFullscreen(bool)));
    connect(largeIconsAct, SIGNAL(toggled(bool)), this, SLOT(toggleLargeIcons(bool)));
    connect(togglePanelAct, SIGNAL(toggled(bool)), this, SLOT(togglePanel(bool)));

    connect(rotateLeftAct, SIGNAL(triggered()), this, SLOT(rotateLeft()));
    connect(rotateRightAct, SIGNAL(triggered()), this, SLOT(rotateRight()));
    connect(rotateResetAct, SIGNAL(triggered()), this, SLOT(rotateReset()));
    connect(zoomInAct, SIGNAL(triggered()), imageDisplay, SLOT(zoomIn()));
    connect(zoomOutAct, SIGNAL(triggered()), imageDisplay, SLOT(zoomOut()));
    connect(zoomResetAct, SIGNAL(triggered()), this, SLOT(zoomReset()));
    connect(fitToWindowAct, SIGNAL(triggered()), imageDisplay, SLOT(fitToScreen()));
    connect(fitToWidthAct, SIGNAL(triggered()), imageDisplay, SLOT(fitWidth()));
    connect(fitToHeightAct, SIGNAL(triggered()), imageDisplay, SLOT(fitHeight()));

    connect(lockNoneAct, SIGNAL(triggered()), this, SLOT(lockNone()));
    connect(lockZoomAct, SIGNAL(triggered()), this, SLOT(lockZoom()));
    connect(lockAutofitAct, SIGNAL(triggered()), this, SLOT(lockAutofit()));
    connect(lockFitHeightAct, SIGNAL(triggered()), this, SLOT(lockFitHeight()));
    connect(lockFitWidthAct, SIGNAL(triggered()), this, SLOT(lockFitWidth()));
}

void MainWindow::openFile(const QString &source)
{
    treeViewFilesystem->setCurrentIndex(fsmTree->index(source));
}

bool MainWindow::checkFileExtension(const QFileInfo &fi)
{
    if(isArchive(fi) || fi.isDir() == true)
    {
       return true;
    }
    else
    {
        return false;
    }
}

bool MainWindow::acceptFileDrop(const QMimeData* mimeData){
     if (mimeData->hasUrls())
     {
         return checkFileExtension(QFileInfo(mimeData->urls().at(0).toLocalFile()));
     }
     else
     {
         return false;
     }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if(acceptFileDrop(event->mimeData()) == true)
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
    // if some actions should not be usable, like move, this code must be adopted
    event->acceptProposedAction();
}


void MainWindow::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    if(event->proposedAction() == Qt::CopyAction)
    {
        openFile(event->mimeData()->urls().at(0).toLocalFile());
    }
    event->acceptProposedAction();
}

void MainWindow::OnPathEdited()
{
    QFileInfo fi(lineEditPath->text());
    bool valid = false;
    if(fi.exists())
    {
        if(checkFileExtension(fi) == true)
        {
            openFile(lineEditPath->text());
            valid = true;
        }
    }

    if(lineEditPath->text().length() == 0) valid = true;

    if(valid == false)
    {
        QApplication::beep();
        QPalette palette( lineEditPath->palette() );
        palette.setColor( QPalette::Base, QColor::fromRgb(255,150,150) );
        lineEditPath->setPalette(palette);
    }
    else
    {
        if(lineEditPath->palette() != QApplication::palette())
        {
            lineEditPath->setPalette(QApplication::palette());
        }
    }
}


void MainWindow::OnTreeViewItemActivated ( const QModelIndex & index )
{
    if(lineEditPath->palette() != QApplication::palette())
    {
        lineEditPath->setPalette(QApplication::palette());

        updatePath(fsmTree->filePath(index));
    }
}

void MainWindow::updatePath(const QString &filePath)
{
    QFileInfo fi(filePath);
    if(fi.isDir())
    {
        lineEditPath->setText(fi.absoluteFilePath() + "/");
    }
    else
    {
        lineEditPath->setText(fi.absoluteFilePath());
    }
}

void MainWindow::refreshPath()
{
    OnTreeViewCurrentChanged(treeViewFilesystem->currentIndex(), treeViewFilesystem->currentIndex());
}

void MainWindow::OnTreeViewCurrentChanged(const QModelIndex & current, const QModelIndex & previous)
{
    dirUpAct->setEnabled(current.parent().isValid());
    treeViewFilesystem->scrollTo(current);
    QString filePath = fsmTree->filePath(current);
    setWindowTitle(fsmTree->fileName(current) + " - " + QApplication::applicationName() + " " + QApplication::applicationVersion());
    updatePath(filePath);

    if(fsmTree->isDir(current))
    {
        fsmTree->fetchMore(current);
        treeViewArchiveDirs->hide();
        fileList->setCurrentDirectory(filePath);
    }
    else
    {
        am->setPath(filePath, true);
        treeViewArchiveDirs->show();
        fileList->setCurrentDirectory(filePath, true);
    }
}

void MainWindow::OnFileListCurrentRowChanged ( const QModelIndex & current, const QModelIndex & previous )
{
    if(current.isValid() == false)
    {
        imageDisplay->setPixmap(NULL);
    }
    else
    {
        loadImageFromWidget(current.data(ROLE_TYPE).toInt(), current.data(Qt::DisplayRole).toString());
    }
}


void MainWindow::togglePanel(bool value)
{
    splitterPanel->setVisible(value);
    toolbarFiles->setVisible(value);
}

void MainWindow::toggleFullscreen(bool value)
{
    togglePanelAct->setChecked(!value);
    if(value == true)
    {

	showFullScreen();
    }
    else
    {
#ifdef linux
	//workaround for bug https://bugreports.qt.nokia.com//browse/QTBUG-10594
	showNormal();
#endif
	showMaximized();
    }
}

void MainWindow::dirUp()
{
    if(treeViewFilesystem->currentIndex().parent().isValid())
    {
	treeViewFilesystem->setCurrentIndex(treeViewFilesystem->currentIndex().parent());
    }
}

void MainWindow::OnImageItemActivated ( const QModelIndex & index )
{
    if(fsmTree->isDir(treeViewFilesystem->currentIndex()))
    {
        if(fsmTree->isDir(index)) //TODO ###OR archive
        {
            treeViewFilesystem->setCurrentIndex(index);
            treeViewFilesystem->expand(treeViewFilesystem->currentIndex());
        }
    }
}

void MainWindow::loadImageFromWidget(int type, const QString& filename)
{
    if(type == TYPE_DIR || type == TYPE_ARCHIVE)
    {
        imageDisplay->setPixmap(NULL);
    }
    else
    {
        QString filepath = getCurrentPath();

        if(fsmTree->isDir(treeViewFilesystem->currentIndex()))
        {
            pl->setFilePath(filepath + "/" + filename);
            threadImage->start();
        }
        else  //TODO ###If archive
        {
//            int SelectedZipFileIndex = getArchiveNumberFromItem(type);

//            if(SelectedZipFileIndex < 0)
//            {
//                imageDisplay->setPixmap(NULL);
//            }
//            else
//            {
//                lp->setFilePath(filepath, true, archive_files.at(SelectedZipFileIndex));
//                threadImage->start();
//            }
        }
    }
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Zip files (*.zip *.cbz)"));
    if (!fileName.isEmpty())
    {
        openFile(fileName);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "untitled.png", tr("Image (*.png)"));
    if (fileName.isEmpty())
    {
        return false;
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    imageDisplay->getPixmap().save(fileName);
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    return true;
}

void MainWindow::zoomReset()
{
    imageDisplay->setZoom(1);
}

void MainWindow::lockZoom()
{
    imageDisplay->setLockMode(LockMode::Zoom);
}

void MainWindow::lockNone()
{
    imageDisplay->setLockMode(LockMode::None);
}

void MainWindow::lockAutofit()
{
    imageDisplay->setLockMode(LockMode::Autofit);
}

void MainWindow::lockFitWidth()
{
    imageDisplay->setLockMode(LockMode::FitWidth);
}

void MainWindow::lockFitHeight()
{
    imageDisplay->setLockMode(LockMode::FitHeight);
}

void MainWindow::rotateLeft()
{
    imageDisplay->setRotation(imageDisplay->getRotation()-10);
}

void MainWindow::rotateRight()
{
    imageDisplay->setRotation(imageDisplay->getRotation()+10);
}

void MainWindow::rotateReset()
{
    imageDisplay->setRotation(0);
}

void MainWindow::settingsDialog()
{
    Settings_Dialog sd(this);
    if(sd.exec() == QDialog::Accepted)
    {
        //update settings
        if(Settings::Instance()->getHardwareAcceleration() != imageDisplay->getHardwareAcceleration())
        {
            imageDisplay->setHardwareAcceleration(Settings::Instance()->getHardwareAcceleration());
            threadImage->start();
        }
    }
}

void MainWindow::toggleLargeIcons(bool value)
{
    int e;
    if(value == true)
    {

//	e = QApplication::style()->pixelMetric(QStyle::PM_LargeIconSize);
	e = QApplication::style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    }
    else
    {
	e = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize);
    }

    QSize iconSize = QSize(e, e);
    toolbar->setIconSize(iconSize);
    toolbarFiles->setIconSize(iconSize);


    Settings::Instance()->setLargeIcons(value);
}

void MainWindow::pageNext()
{
//    if(treeViewArchiveDirs->currentItem() == NULL) return;

//    if(treeViewArchiveDirs->currentItem()->parent() == NULL)
//    {
//        int curr = treeViewArchiveDirs->indexOfTopLevelItem(treeViewArchiveDirs->currentItem());
//        int max = treeViewArchiveDirs->topLevelItemCount();

//        for(int i = curr + 1; i < max; i++)
//        {
//            if(treeViewArchiveDirs->topLevelItem(i)->type() == TYPE_FILE)
//            {
//                treeViewArchiveDirs->setCurrentItem(treeViewArchiveDirs->topLevelItem(i));
//                break;
//            }
//        }

//    }
//    else
//    {
//        QTreeWidgetItem* ctwip = treeViewArchiveDirs->currentItem()->parent();
//        int curr = ctwip->indexOfChild(treeViewArchiveDirs->currentItem());
//        int max = ctwip->childCount();

//        for(int i = curr + 1; i < max; i++)
//        {
//            if(ctwip->child(i)->type() != TYPE_DIR)
//            {
//                treeViewArchiveDirs->setCurrentItem(ctwip->child(i));
//                break;
//            }
//        }
//    }
}

void MainWindow::pagePrevious()
{
//    if(treeViewArchiveDirs->currentItem() == NULL) return;
//    if(treeViewArchiveDirs->currentItem()->parent() == NULL)
//    {
//        int curr = treeViewArchiveDirs->indexOfTopLevelItem(treeViewArchiveDirs->currentItem());

//        for(int i = curr - 1; i >= 0; i--){
//            if(treeViewArchiveDirs->topLevelItem(i)->type() == TYPE_FILE) {
//                treeViewArchiveDirs->setCurrentItem(treeViewArchiveDirs->topLevelItem(i));
//                break;
//            }
//        }

//    }
//    else
//    {
//        QTreeWidgetItem* ctwip = treeViewArchiveDirs->currentItem()->parent();
//        int curr = ctwip->indexOfChild(treeViewArchiveDirs->currentItem());

//        for(int i = curr - 1; i >= 0; i--){
//            if(ctwip->child(i)->type() != TYPE_DIR) {
//                treeViewArchiveDirs->setCurrentItem(ctwip->child(i));
//                break;
//            }
//        }
//    }
}

void MainWindow::OnZoomChanged()
{
    QString zoomText = QString::number((int)(imageDisplay->getZoom() * 100)) + "%";
    comboBoxZoom->setEditText(zoomText);
}

bool MainWindow::parseZoom(const QString &zoomText)
{
    QString zoomvalue = zoomText;
    zoomvalue = zoomvalue.remove('%');

    bool ok;
    int dec = zoomvalue.toInt(&ok, 10);

    if(ok)
    {
        qreal z = (qreal)dec / 100;
        imageDisplay->setZoom(z);
    }

    return ok;
}

void MainWindow::OnComboBoxZoomTextChanged()
{
    if(parseZoom(comboBoxZoom->lineEdit()->text()) == false)
    {
        OnZoomChanged();
    }
}

void MainWindow::OnComboBoxZoomIndexChanged(const int &index)
{
    imageDisplay->setZoom(imageDisplay->getDefaultZoomSizes().at(index));
}

void MainWindow::toggleShowThumbnails(bool)
{
    if(showThumbnailsAct->isChecked() == true)
    {
        fileList->setViewMode(QListView::IconMode);
    }
    else
    {
        fileList->setViewMode(QListView::ListMode);
    }
}

void MainWindow::OnPixmapLoaderFinished(QPixmap p)
{
    imageDisplay->setPixmap(p);
    threadImage->exit();

    bool enableActions = !imageDisplay->getPixmap().isNull();

    saveAct->setEnabled(enableActions);
    zoomInAct->setEnabled(enableActions);
    zoomOutAct->setEnabled(enableActions);
    rotateLeftAct->setEnabled(enableActions);
    rotateRightAct->setEnabled(enableActions);
    zoomResetAct->setEnabled(enableActions);
    fitToWindowAct->setEnabled(enableActions);
    fitToWidthAct->setEnabled(enableActions);
    fitToHeightAct->setEnabled(enableActions);
    rotateResetAct->setEnabled(enableActions);
    comboBoxZoom->setEnabled(enableActions);
}
