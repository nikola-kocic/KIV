#include "main_window.h"
#include "settings_dialog.h"
#include "system_icons.h"
#include "settings.h"
#include "teximg.h"
#include "quazip/JlCompress.h"

//#include <QtCore/qdebug.h>
#include <QtCore/qbuffer.h>
#include <QtCore/QUrl>
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
#include <QtGui/qmessagebox.h>
#include <QtGui/qdesktopservices.h>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags f)
{
    this->resize(QApplication::desktop()->width() - 100,
                 QApplication::desktop()->height() - 100);
    this->setWindowTitle(QApplication::applicationName() + " " + QApplication::applicationVersion());

    this->setWindowIcon(QIcon(":/icons/komicviewer.svg"));

    this->modelFilesystem = new QFileSystemModel(this);
    QStringList filters;

    foreach (const QString &ext, Settings::Instance()->getFiltersArchive())
    {
        filters << "*." + ext;
    }

    this->modelFilesystem->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    this->modelFilesystem->setNameFilterDisables(false);
    this->modelFilesystem->setNameFilters(filters);
    this->modelFilesystem->setRootPath("");

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    policy.setHorizontalStretch(1);
    policy.setVerticalStretch(0);

    this->modelArchive = new ArchiveModel();
    this->createActions();

    this->splitterMain = new QSplitter(Qt::Horizontal, this);

    QVBoxLayout *vboxMain = new QVBoxLayout(this);
    vboxMain->setSpacing(0);
    vboxMain->setMargin(0);


    QMenuBar *mainMenu = new QMenuBar();
    mainMenu->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    this->createMenus(mainMenu);

    this->lineEditPath = new QLineEdit();

    this->comboBoxZoom = new QComboBox();
    this->comboBoxZoom->setEnabled(false);
    this->comboBoxZoom->setInsertPolicy(QComboBox::NoInsert);
    this->comboBoxZoom->setMaxVisibleItems(12);
    this->comboBoxZoom->setMinimumContentsLength(8);
    this->comboBoxZoom->setEditable(true);
    this->comboBoxZoom->setFocusPolicy(Qt::ClickFocus);

    this->toolbar = new QToolBar();
    this->toolbar->setMovable(false);
    this->toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
//    toolbar->setIconSize(QSize(16,16));
    this->toolbar->layout()->setMargin(0);

    this->toolbar->addAction(this->togglePanelAct);
    this->toolbar->addAction(this->toggleFullscreenAct);
    this->toolbar->addSeparator();
    this->toolbar->addWidget(mainMenu);
    this->toolbar->addSeparator();
    this->toolbar->addAction(this->pagePreviousAct);
    this->toolbar->addAction(this->pageNextAct);
    this->toolbar->addSeparator();
    this->toolbar->addAction(this->zoomInAct);
    this->toolbar->addAction(this->zoomOutAct);
    this->toolbar->addAction(this->fitToWindowAct);
    this->toolbar->addWidget(this->comboBoxZoom);
    this->toolbar->addSeparator();
    this->toolbar->addAction(this->rotateLeftAct);
    this->toolbar->addAction(this->rotateRightAct);

    this->toolbarDirectory = new QToolBar();
    this->toolbarDirectory->setMovable(false);
    this->toolbarDirectory->setContextMenuPolicy(Qt::PreventContextMenu);
    this->toolbarDirectory->addAction(this->dirUpAct);
    this->toolbarDirectory->addAction(this->refreshPathAct);
    this->toolbarDirectory->addWidget(this->lineEditPath);

    vboxMain->addWidget(this->toolbar);
    vboxMain->addWidget(this->toolbarDirectory);

    //Panel start

    this->splitterPanel = new QSplitter(Qt::Vertical, this);

    this->filesystemView = new QTreeView();
    this->filesystemView->setUniformRowHeights(true);
    this->filesystemView->setHeaderHidden(true);

    this->splitterPanel->addWidget(this->filesystemView);

    QSplitter *splitterFiles = new QSplitter(Qt::Vertical, this);

    this->archiveDirsView = new ViewArchiveDirs();
    this->archiveDirsView->hide();

    splitterFiles->addWidget(this->archiveDirsView);
    splitterFiles->setSizes(QList<int>() << 100);

    this->filesView = new ViewFiles();
    this->filesView->setModel(modelArchive);

    QSizePolicy policyV(QSizePolicy::Preferred, QSizePolicy::Expanding);
    policyV.setHorizontalStretch(0);
    policyV.setVerticalStretch(1);

    this->filesView->setSizePolicy(policyV);
    splitterFiles->addWidget(this->filesView);

    this->splitterPanel->addWidget(splitterFiles);

    this->splitterMain->addWidget(this->splitterPanel);

    this->splitterMain->setSizes(QList<int>() << 300);

    //Panel end


    //Content start
    this->imageDisplay = new PictureItem(Settings::Instance()->getHardwareAcceleration());
    this->imageDisplay->setSizePolicy(policy);

    this->splitterMain->addWidget(this->imageDisplay);
    //Content end


    vboxMain->addWidget(this->splitterMain);
    setLayout(vboxMain);

    this->filesystemView->setModel(this->modelFilesystem);
    for (int i = 1; i < this->filesystemView->header()->count(); ++i)
    {
        this->filesystemView->hideColumn(i);
    }

    this->archiveDirsView->setModel(this->modelArchive);



    foreach (const qreal &i, this->imageDisplay->getDefaultZoomSizes())
    {
        this->comboBoxZoom->addItem(QString::number((int)(i * 100)) + "%");
        if (i == 1)
        {
            this->comboBoxZoom->setCurrentIndex(this->comboBoxZoom->count() - 1);
        }
    }

    // Now add the line to the splitter handle
    // Note: index 0 handle is always hidden, index 1 is between the two widgets
    QSplitterHandle *handleMain = this->splitterMain->handle(1);
    QVBoxLayout *layoutMain = new QVBoxLayout(handleMain);
    layoutMain->setSpacing(0);
    layoutMain->setMargin(0);
    QFrame *frameMain = new QFrame(handleMain);
    frameMain->setFrameShape(QFrame::Panel);
    frameMain->setFrameShadow(QFrame::Raised);
    layoutMain->addWidget(frameMain);

    // Now add the line to the splitter handle
    // Note: index 0 handle is always hidden, index 1 is between the two widgets
    QSplitterHandle *handlePanel = this->splitterPanel->handle(1);
    QVBoxLayout *layoutPanel = new QVBoxLayout(handlePanel);
    layoutPanel->setSpacing(0);
    layoutPanel->setMargin(0);
    QFrame *framePanel = new QFrame(handlePanel);
    framePanel->setFrameShape(QFrame::Panel);
    framePanel->setFrameShadow(QFrame::Raised);
    layoutPanel->addWidget(framePanel);


    //Large icons are On by default but I want small icons by default
    if (Settings::Instance()->getLargeIcons())
    {
        this->largeIconsAct->setChecked(true);
    }
    else
    {
        toggleLargeIcons(false);
    }

    this->connectActions();

    QCompleter *completer = new QCompleter(this);
    completer->setModel(this->modelFilesystem);
    this->lineEditPath->setCompleter(completer);

    if (QApplication::arguments().count() > 1)
    {
        this->openFile(QApplication::arguments().at(1));
    }
    else
    {
        QString path = Settings::Instance()->getLastPath();
        if (!path.isEmpty())
        {
            this->filesystemView->setCurrentIndex(this->modelFilesystem->index(path));
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Settings::Instance()->setLastPath(filesView->getCurrentFileInfo().containerPath);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if (this->lineEditPath->hasFocus())
	{
//	    if (lineEditPath->palette() != QApplication::palette())
//	    {
//		lineEditPath->setPalette(QApplication::palette());
//	    }

//	    updatePath(fsm->filePath(index));

	    refreshPath();

	    event->accept();
	}
        else if (this->comboBoxZoom->lineEdit()->hasFocus())
	{
	    OnZoomChanged();
	    event->accept();
	}
    }
}

void MainWindow::createActions()
{
    static const char * GENERIC_ICON_TO_CHECK = "media-skip-backward";
    static const char * FALLBACK_ICON_THEME = "glyphs";
    if (!QIcon::hasThemeIcon(GENERIC_ICON_TO_CHECK)) {
        //If there is no default working icon theme then we should
        //use an icon theme that we provide via a icons folder
        //This case happens under Windows and Mac OS X
        //This does not happen under GNOME or KDE
//        QIcon::setThemeSearchPaths(QStringList() << QCoreApplication::applicationDirPath());
        QIcon::setThemeName(FALLBACK_ICON_THEME);
    }

    this->openAct = new QAction(QIcon::fromTheme("document-open"), tr("&Open..."), this);
    this->openAct->setShortcut(tr("Ctrl+O"));

    this->saveAct = new QAction(QIcon::fromTheme("document-save-as"), tr("&Save Page As..."), this);
    this->saveAct->setShortcut(tr("Ctrl+S"));
    this->saveAct->setEnabled(false);

    //    printAct = new QAction(tr("&Print..."), this);
    //    printAct->setShortcut(tr("Ctrl+P"));
    //    printAct->setEnabled(false);
    //    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    this->exitAct = new QAction(QIcon::fromTheme("application-exit"), tr("E&xit"), this);
    this->exitAct->setShortcut(tr("Ctrl+Q"));

    //Options Actions

    this->pagePreviousAct = new QAction(QIcon::fromTheme("media-skip-backward"), tr("&Previous Page"), this);
    this->pagePreviousAct->setShortcut(Qt::Key_PageUp);

    this->pageNextAct = new QAction(QIcon::fromTheme("media-skip-forward"), tr("&Next Page"), this);
    this->pageNextAct->setShortcut(Qt::Key_PageDown);

    this->rotateLeftAct = new QAction(QIcon::fromTheme("object-rotate-left"), tr("Rotate &Left"), this);
    this->rotateLeftAct->setEnabled(false);

    this->rotateRightAct = new QAction(QIcon::fromTheme("object-rotate-right"), tr("Rotate &Right"), this);
    this->rotateRightAct->setEnabled(false);

    this->rotateResetAct = new QAction(tr("R&eset Rotation"), this);
    this->rotateResetAct->setEnabled(false);

    this->showThumbnailsAct = new QAction(QIcon::fromTheme("view-list-icons"), tr("&Show Thumbnails"), this);
    this->showThumbnailsAct->setCheckable(true);

    this->toggleFullscreenAct = new QAction(QIcon::fromTheme("view-fullscreen"), tr("&Full Screen"), this);
    this->toggleFullscreenAct->setShortcut(Qt::Key_F11);
    this->toggleFullscreenAct->setCheckable(true);

    this->togglePanelAct = new QAction(QIcon::fromTheme("view-split-left-right"),tr("Show Side&bar"), this);
    this->togglePanelAct->setCheckable(true);
    this->togglePanelAct->setChecked(true);

    this->largeIconsAct = new QAction(tr("Large &Icons"), this);
    this->largeIconsAct->setCheckable(true);

    this->settingsAct = new QAction(QIcon::fromTheme("configure", QIcon::fromTheme("gtk-preferences")), tr("&Settings..."), this);
    this->settingsAct->setMenuRole(QAction::PreferencesRole);
//    settingsAct->setVisible(false);



    //Zoom Actions

    this->zoomInAct = new QAction(QIcon::fromTheme("zoom-in"), tr("Zoom &In"), this);
    this->zoomInAct->setShortcut(Qt::CTRL | Qt::Key_Plus);
    this->zoomInAct->setEnabled(false);

    this->zoomOutAct = new QAction(QIcon::fromTheme("zoom-out"), tr("Zoom &Out"), this);
    this->zoomOutAct->setShortcut(Qt::CTRL | Qt::Key_Minus);
    this->zoomOutAct->setEnabled(false);

    this->zoomResetAct = new QAction(QIcon::fromTheme("zoom-original"), tr("&Original Size (100%)"), this);
    this->zoomResetAct->setShortcut(Qt::CTRL | Qt::Key_1);
    this->zoomResetAct->setEnabled(false);

    this->fitToWindowAct = new QAction(QIcon::fromTheme("zoom-fit-best"), tr("&Fit to Window"), this);
    this->fitToWindowAct->setShortcut(Qt::CTRL | Qt::Key_T);
    this->fitToWindowAct->setEnabled(false);

    this->fitToWidthAct = new QAction(QIcon::fromTheme("zoom-fit-width"), tr("Fit &Width"), this);
    this->fitToWidthAct->setEnabled(false);

    this->fitToHeightAct = new QAction(QIcon::fromTheme("zoom-fit-height"), tr("Fit &Height"), this);
    this->fitToHeightAct->setEnabled(false);

    this->lockNoneAct = new QAction(tr("Lock &None"), this);
    this->lockNoneAct->setCheckable(true);
    this->lockNoneAct->setChecked(true);

    this->lockAutofitAct = new QAction(tr("Lock Autofi&t"), this);
    this->lockAutofitAct->setCheckable(true);

    this->lockFitHeightAct = new QAction(tr("Lock Fit Hei&ght"), this);
    this->lockFitHeightAct->setCheckable(true);

    this->lockFitWidthAct = new QAction(tr("Lock Fit Wi&dth"), this);
    this->lockFitWidthAct->setCheckable(true);

    this->lockZoomAct = new QAction(tr("Lock Zoom &Value"), this);
    this->lockZoomAct->setCheckable(true);

    this->webSiteAct = new QAction(tr("&Web Site"), this);

    this->aboutAct = new QAction(tr("&About"), this);
    this->aboutAct->setShortcut(Qt::Key_F1);

    this->refreshPathAct = new QAction(QIcon::fromTheme("view-refresh"), tr("&Refresh"), this);

    this->dirUpAct = new QAction(QIcon::fromTheme("go-up"), tr("Go &Up"), this);
    this->dirUpAct->setEnabled(false);
}

void MainWindow::createMenus(QMenuBar *parent)
{
    QMenu *fileMenu = new QMenu(tr("&File"), parent);
    fileMenu->addAction(this->openAct);
    fileMenu->addAction(this->saveAct);
//    fileMenu->addAction(printAct);
//    fileMenu->addSeparator();
    fileMenu->addAction(this->exitAct);

    QMenu *editMenu = new QMenu(tr("&Edit"), parent);
    editMenu->addAction(this->rotateLeftAct);
    editMenu->addAction(this->rotateRightAct);
    editMenu->addAction(this->rotateResetAct);
    editMenu->addSeparator();
    editMenu->addAction(this->zoomInAct);
    this->addAction(this->zoomInAct);
    editMenu->addAction(this->zoomOutAct);
    this->addAction(this->zoomOutAct);
    editMenu->addAction(this->zoomResetAct);
    this->addAction(this->zoomResetAct);
    editMenu->addSeparator();
    editMenu->addAction(this->fitToWindowAct);
    this->addAction(this->fitToWindowAct);
    editMenu->addAction(this->fitToWidthAct);
    editMenu->addAction(this->fitToHeightAct);
    editMenu->addSeparator();

    QActionGroup *lockActions = new QActionGroup(this);
    lockActions->addAction(this->lockNoneAct);
    lockActions->addAction(this->lockZoomAct);
    lockActions->addAction(this->lockAutofitAct);
    lockActions->addAction(this->lockFitWidthAct);
    lockActions->addAction(this->lockFitHeightAct);

    editMenu->addAction(this->lockNoneAct);
    editMenu->addAction(this->lockZoomAct);
    editMenu->addAction(this->lockAutofitAct);
    editMenu->addAction(this->lockFitWidthAct);
    editMenu->addAction(this->lockFitHeightAct);


    QMenu *optionsMenu = new QMenu(tr("&Options"), parent);
    this->addAction(this->pagePreviousAct);
    this->addAction(this->pageNextAct);
    optionsMenu->addAction(this->pagePreviousAct);
    optionsMenu->addAction(this->pageNextAct);
    optionsMenu->addSeparator();
    optionsMenu->addAction(this->showThumbnailsAct);
    this->addAction(this->toggleFullscreenAct);
    optionsMenu->addAction(this->toggleFullscreenAct);
    optionsMenu->addAction(this->togglePanelAct);
    optionsMenu->addAction(this->largeIconsAct);
    optionsMenu->addAction(this->settingsAct);

    QMenu *helpMenu = new QMenu(tr("&Help"), parent);
    helpMenu->addAction(this->webSiteAct);
    helpMenu->addAction(this->aboutAct);

    parent->addMenu(fileMenu);
    parent->addMenu(editMenu);
    parent->addMenu(optionsMenu);
    parent->addMenu(helpMenu);
}

void MainWindow::connectActions()
{
    connect(this->openAct, SIGNAL(triggered()), this, SLOT(open()));
    connect(this->saveAct, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(this->exitAct, SIGNAL(triggered()), this, SLOT(close()));

    connect(this->settingsAct, SIGNAL(triggered()), this, SLOT(settingsDialog()));
    connect(this->pagePreviousAct, SIGNAL(triggered()), this->filesView, SLOT(pagePrevious()));
    connect(this->pageNextAct, SIGNAL(triggered()), this->filesView, SLOT(pageNext()));


    connect(this->showThumbnailsAct, SIGNAL(toggled(bool)), this, SLOT(toggleShowThumbnails(bool)));
    connect(this->toggleFullscreenAct, SIGNAL(toggled(bool)), this, SLOT(toggleFullscreen(bool)));
    connect(this->largeIconsAct, SIGNAL(toggled(bool)), this, SLOT(toggleLargeIcons(bool)));
    connect(this->togglePanelAct, SIGNAL(toggled(bool)), this, SLOT(togglePanel(bool)));

    connect(this->rotateLeftAct, SIGNAL(triggered()), this, SLOT(rotateLeft()));
    connect(this->rotateRightAct, SIGNAL(triggered()), this, SLOT(rotateRight()));
    connect(this->rotateResetAct, SIGNAL(triggered()), this, SLOT(rotateReset()));
    connect(this->zoomInAct, SIGNAL(triggered()), this->imageDisplay, SLOT(zoomIn()));
    connect(this->zoomOutAct, SIGNAL(triggered()), this->imageDisplay, SLOT(zoomOut()));
    connect(this->zoomResetAct, SIGNAL(triggered()), this, SLOT(zoomReset()));
    connect(this->fitToWindowAct, SIGNAL(triggered()), this->imageDisplay, SLOT(fitToScreen()));
    connect(this->fitToWidthAct, SIGNAL(triggered()), this->imageDisplay, SLOT(fitWidth()));
    connect(this->fitToHeightAct, SIGNAL(triggered()), this->imageDisplay, SLOT(fitHeight()));

    connect(this->lockNoneAct, SIGNAL(triggered()), this, SLOT(lockNone()));
    connect(this->lockZoomAct, SIGNAL(triggered()), this, SLOT(lockZoom()));
    connect(this->lockAutofitAct, SIGNAL(triggered()), this, SLOT(lockAutofit()));
    connect(this->lockFitHeightAct, SIGNAL(triggered()), this, SLOT(lockFitHeight()));
    connect(this->lockFitWidthAct, SIGNAL(triggered()), this, SLOT(lockFitWidth()));

    connect(this->aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    connect(this->webSiteAct, SIGNAL(triggered()), this, SLOT(website()));


    connect(this->filesView, SIGNAL(activated(QModelIndex)), this, SLOT(OnFilesViewItemActivated(QModelIndex)));
    connect(this->lineEditPath, SIGNAL(editingFinished()), this, SLOT(OnPathEdited()));
    connect(this->filesystemView, SIGNAL(clicked(QModelIndex)), this, SLOT(OnTreeViewItemActivated(QModelIndex)));

    connect(this->filesystemView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(OnTreeViewCurrentChanged(QModelIndex,QModelIndex)));
    connect(this->archiveDirsView, SIGNAL(currentRowChanged(QModelIndex)), this->filesView, SLOT(OnTreeViewArchiveDirsCurrentChanged(QModelIndex)));

    connect(this->filesView, SIGNAL(currentFileChanged(FileInfo)), this, SLOT(OnFilesViewCurrentChanged(FileInfo)));
    connect(this->imageDisplay, SIGNAL(imageChanged()), this, SLOT(updateActions()));

    connect(this->imageDisplay, SIGNAL(toggleFullscreen()), this->toggleFullscreenAct, SLOT(toggle()));
    connect(this->imageDisplay, SIGNAL(pageNext()), this->filesView, SLOT(pageNext()));
    connect(this->imageDisplay, SIGNAL(pagePrevious()), this->filesView, SLOT(pagePrevious()));
    connect(this->imageDisplay, SIGNAL(zoomChanged()), this, SLOT(OnZoomChanged()));
    connect(this->comboBoxZoom, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxZoomIndexChanged(int)));
    connect(this->comboBoxZoom->lineEdit(), SIGNAL(returnPressed()), this, SLOT(OnComboBoxZoomTextChanged()));

    connect(this->refreshPathAct, SIGNAL(triggered()), this, SLOT(refreshPath()));
    connect(this->dirUpAct, SIGNAL(triggered()), this, SLOT(dirUp()));

}

void MainWindow::OnFilesViewCurrentChanged(const FileInfo &info)
{
    this->setCursor(Qt::BusyCursor);
    this->imageDisplay->setPixmap(info);
}

void MainWindow::openFile(const QString &source)
{
    this->filesystemView->setCurrentIndex(this->modelFilesystem->index(source));
}


bool MainWindow::acceptFileDrop(const QMimeData *mimeData){
     if (mimeData->hasUrls())
     {
         return checkFileExtension(QFileInfo(mimeData->urls().at(0).toLocalFile()));
     }
     else
     {
         return false;
     }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (this->acceptFileDrop(event->mimeData()))
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    // if some actions should not be usable, like move, this code must be adopted
    event->acceptProposedAction();
}


void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->proposedAction() == Qt::CopyAction)
    {
        this->openFile(event->mimeData()->urls().at(0).toLocalFile());
    }
    event->acceptProposedAction();
}

void MainWindow::OnPathEdited()
{
    QFileInfo fi(this->lineEditPath->text());
    bool valid = false;
    if (fi.exists())
    {
        if (checkFileExtension(fi))
        {
            this->openFile(this->lineEditPath->text());
            valid = true;
        }
    }

    if (this->lineEditPath->text().length() == 0) valid = true;

    if (!valid)
    {
        QApplication::beep();
        QPalette palette(this->lineEditPath->palette());
        palette.setColor(QPalette::Base, QColor::fromRgb(255,150,150));
        this->lineEditPath->setPalette(palette);
    }
    else
    {
        if (this->lineEditPath->palette() != QApplication::palette())
        {
            this->lineEditPath->setPalette(QApplication::palette());
        }
    }
}

void MainWindow::updatePath(const QString &filePath)
{
    QFileInfo fi(filePath);
    if (fi.isDir())
    {
        this->lineEditPath->setText(fi.absoluteFilePath() + "/");
    }
    else
    {
        this->lineEditPath->setText(fi.absoluteFilePath());
    }
}

void MainWindow::refreshPath()
{
    OnTreeViewCurrentChanged(this->filesystemView->currentIndex(), this->filesystemView->currentIndex());
}

void MainWindow::OnTreeViewItemActivated(const QModelIndex &index)
{
    if (this->lineEditPath->palette() != QApplication::palette())
    {
        this->lineEditPath->setPalette(QApplication::palette());

        this->updatePath(this->modelFilesystem->filePath(index));
    }
}

void MainWindow::OnTreeViewCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    this->dirUpAct->setEnabled(current.parent().isValid());
    this->filesystemView->scrollTo(current);
    QString currentFolder = this->modelFilesystem->filePath(current);
    this->setWindowTitle(this->modelFilesystem->filePath(current) + " - " + QApplication::applicationName() + " " + QApplication::applicationVersion());
    this->updatePath(currentFolder);

    FileInfo info;
    info.containerPath = currentFolder;

    if (this->modelFilesystem->isDir(current))
    {
        this->modelFilesystem->fetchMore(current);
    }
    else
    {
        info.zipPathToImage = "/";
    }

    this->modelArchive->setPath(info);

    if (info.zipPathToImage.isEmpty())
    {
        this->archiveDirsView->hide();
    }
    else
    {
        this->archiveDirsView->show();
    }

    this->filesView->setCurrentDirectory(info);

    this->imageDisplay->setPixmap(info);
}

void MainWindow::OnFilesViewItemActivated(const QModelIndex &index)
{
    int type = index.data(ROLE_TYPE).toInt();
    if (type == TYPE_DIR || type == TYPE_ARCHIVE)
    {
        this->filesystemView->setCurrentIndex(this->modelFilesystem->index(filesView->getCurrentFileInfo().containerPath + "/" + index.data(Qt::DisplayRole).toString()));
        this->filesystemView->expand(this->filesystemView->currentIndex());
    }
    else if (type == TYPE_ARCHIVE_DIR)
    {
        this->archiveDirsView->setCurrentIndexFromSource(this->filesView->getIndexFromProxy(index));
    }
}

void MainWindow::togglePanel(bool value)
{
    this->splitterPanel->setVisible(value);
    this->toolbarDirectory->setVisible(value);
}

void MainWindow::toggleFullscreen(bool value)
{
    this->togglePanelAct->setChecked(!value);
    if (value)
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
    if (this->filesystemView->currentIndex().parent().isValid())
    {
        this->filesystemView->setCurrentIndex(this->filesystemView->currentIndex().parent());
    }
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Zip files (*.zip *.cbz)"));
    if (!fileName.isEmpty())
    {
        this->openFile(fileName);
    }
}

bool MainWindow::saveAs()
{
    FileInfo info = this->filesView->getCurrentFileInfo();
    QString tempFileName;


    if(info.zipImageFileName.isEmpty())
    {
        tempFileName = info.imageFileName;
    }
    else
    {
        tempFileName = info.zipImageFileName;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), tempFileName);
    if (fileName.isEmpty())
    {
        return false;
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    if(info.zipImageFileName.isEmpty())
    {
        QFile::copy(info.containerPath + "/" + info.imageFileName, fileName);
    }
    else
    {
        JlCompress::extractFile(info.containerPath, (info.zipPathToImage.compare("/") == 0 ? "" : info.zipPathToImage) + info.zipImageFileName, fileName);
    }
    PictureLoader::getImage(this->filesView->getCurrentFileInfo()).save(fileName, "PNG");
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    return true;
}

void MainWindow::zoomReset()
{
    this->imageDisplay->setZoom(1);
}

void MainWindow::lockZoom()
{
    this->imageDisplay->setLockMode(LockMode::Zoom);
}

void MainWindow::lockNone()
{
    this->imageDisplay->setLockMode(LockMode::None);
}

void MainWindow::lockAutofit()
{
    this->imageDisplay->setLockMode(LockMode::Autofit);
}

void MainWindow::lockFitWidth()
{
    this->imageDisplay->setLockMode(LockMode::FitWidth);
}

void MainWindow::lockFitHeight()
{
    this->imageDisplay->setLockMode(LockMode::FitHeight);
}

void MainWindow::rotateLeft()
{
    this->imageDisplay->setRotation(this->imageDisplay->getRotation() - 10);
}

void MainWindow::rotateRight()
{
    this->imageDisplay->setRotation(this->imageDisplay->getRotation() + 10);
}

void MainWindow::rotateReset()
{
    this->imageDisplay->setRotation(0);
}

void MainWindow::settingsDialog()
{
    Settings_Dialog sd(this);
    if (sd.exec() == QDialog::Accepted)
    {
        //update settings
        if (Settings::Instance()->getHardwareAcceleration() != this->imageDisplay->getHardwareAcceleration())
        {
            this->imageDisplay->setHardwareAcceleration(Settings::Instance()->getHardwareAcceleration());
            this->imageDisplay->setPixmap(filesView->getCurrentFileInfo());
        }
    }
}

void MainWindow::about()
{
    QMessageBox *aboutBox = new QMessageBox();
    aboutBox->setWindowTitle("About KomicViewer");
    aboutBox->setText(
                QApplication::applicationName() +
                tr(" version ") + QApplication::applicationVersion() +
                trUtf8("\n\nAuthor: Nikola Kocić (nikolakocic@gmail.com)") +
                "\n\nWebsite: http://nikola-kocic.github.com/KomicViewer/"
                );
    aboutBox->exec();
}

void MainWindow::website()
{
    QDesktopServices::openUrl(QUrl("http://nikola-kocic.github.com/KomicViewer/"));
}

void MainWindow::toggleLargeIcons(bool value)
{
    int e;
    if (value)
    {

//	e = QApplication::style()->pixelMetric(QStyle::PM_LargeIconSize);
	e = QApplication::style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    }
    else
    {
	e = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize);
    }

    QSize iconSize = QSize(e, e);
    this->toolbar->setIconSize(iconSize);
    this->toolbarDirectory->setIconSize(iconSize);


    Settings::Instance()->setLargeIcons(value);
}

void MainWindow::OnZoomChanged()
{
    QString zoomText = QString::number((int)(this->imageDisplay->getZoom() * 100)) + "%";
    this->comboBoxZoom->setEditText(zoomText);
}

bool MainWindow::parseZoom(const QString &zoomText)
{
    QString zoomvalue = zoomText;
    zoomvalue = zoomvalue.remove('%');

    bool ok;
    int dec = zoomvalue.toInt(&ok, 10);

    if (ok)
    {
        qreal z = (qreal)dec / 100;
        this->imageDisplay->setZoom(z);
    }

    return ok;
}

void MainWindow::OnComboBoxZoomTextChanged()
{
    if (!this->parseZoom(this->comboBoxZoom->lineEdit()->text()))
    {
        OnZoomChanged();
    }
}

void MainWindow::OnComboBoxZoomIndexChanged(const int &index)
{
    this->imageDisplay->setZoom(this->imageDisplay->getDefaultZoomSizes().at(index));
}

void MainWindow::toggleShowThumbnails(bool)
{
    if (this->showThumbnailsAct->isChecked())
    {
        this->filesView->setViewMode(QListView::IconMode);
    }
    else
    {
        this->filesView->setViewMode(QListView::ListMode);
    }
}

void MainWindow::updateActions()
{
    this->unsetCursor();

    bool enableActions = !this->imageDisplay->isPixmapNull();

    this->saveAct->setEnabled(enableActions);
    this->zoomInAct->setEnabled(enableActions);
    this->zoomOutAct->setEnabled(enableActions);
    this->rotateLeftAct->setEnabled(enableActions);
    this->rotateRightAct->setEnabled(enableActions);
    this->zoomResetAct->setEnabled(enableActions);
    this->fitToWindowAct->setEnabled(enableActions);
    this->fitToWidthAct->setEnabled(enableActions);
    this->fitToHeightAct->setEnabled(enableActions);
    this->rotateResetAct->setEnabled(enableActions);
    this->comboBoxZoom->setEnabled(enableActions);
}
