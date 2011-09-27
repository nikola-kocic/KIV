#include "KomicViewer.h"
#include "settings_dialog.h"
#include "system_icons.h"
#include "settings.h"

//#include <QtCore/qdebug.h>
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

KomicViewer::KomicViewer (QStringList args, QWidget * parent, Qt::WindowFlags f)
{
    resize(QApplication::desktop()->width() - 100,
		QApplication::desktop()->height() - 100);
    setWindowTitle(QApplication::applicationName() + " " + QApplication::applicationVersion());

    setWindowIcon(QIcon(":/icons/komicviewer.svg"));


    createActions();

    QSplitter *splitterMain = new QSplitter(Qt::Horizontal, this);

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

    treeWidgetFiles = new QTreeWidget();
    treeWidgetFiles->setHeaderHidden(true);
    treeWidgetFiles->header()->setResizeMode(QHeaderView::Stretch);
    treeWidgetFiles->setUniformRowHeights(true);
    treeWidgetFiles->setHeaderLabels(QStringList() << "Name");

    splitterPanel->addWidget(treeWidgetFiles);

    splitterMain->addWidget(splitterPanel);

    splitterMain->setSizes(QList<int>() << 300);

    //Panel end


    //Content start

    imageDisplay = new PictureItem(Settings::Instance()->getHardwareAcceleration());

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    policy.setHorizontalStretch(1);
    policy.setVerticalStretch(0);
    imageDisplay->setSizePolicy(policy);

    splitterMain->addWidget(imageDisplay);

    //Content end


    vboxMain->addWidget(splitterMain);
    setLayout(vboxMain);

    fsm = new QFileSystemModel(this);
    filters_archive << "zip" << "cbz";
    QStringList filters;
    for(int i = 0; i < filters_archive.count(); i++)
    {
        filters << "*." + filters_archive.at(i);
    }
    fsm->setNameFilterDisables(false);
    fsm->setNameFilters(filters);
    fsm->setRootPath("");
    treeViewFilesystem->setModel(fsm);
    for(int i = 1; i < treeViewFilesystem->header()->count(); i++) treeViewFilesystem->hideColumn(i);

    foreach (const QByteArray &ext, QImageReader::supportedImageFormats()) filters_image << ext;

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
    connect(lineEditPath, SIGNAL(editingFinished()), this, SLOT(OnPathEdited()));
    connect(treeViewFilesystem->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnTreeViewCurrentChanged(QModelIndex,QModelIndex)));
    connect(treeViewFilesystem, SIGNAL(clicked(QModelIndex)), this, SLOT(OnTreeViewItemActivated(QModelIndex)));
    connect(treeWidgetFiles, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(OnTreeFileWidgetCurrentChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(treeWidgetFiles, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(OnTreeFileWidgetItemActivated(QTreeWidgetItem*,int)));
    connect(imageDisplay, SIGNAL(toggleFullscreen()), toggleFullscreenAct, SLOT(toggle()));
    connect(imageDisplay, SIGNAL(pageNext()), this, SLOT(pageNext()));
    connect(imageDisplay, SIGNAL(pagePrevious()), this, SLOT(pagePrevious()));
    connect(imageDisplay, SIGNAL(zoomChanged()), this, SLOT(OnZoomChanged()));
    connect(comboBoxZoom, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxZoomIndexChanged(int)));
    connect(comboBoxZoom->lineEdit(), SIGNAL(returnPressed()), this, SLOT(OnComboBoxZoomTextChanged()));

    connect(refreshPathAct, SIGNAL(triggered()), this, SLOT(refreshPath()));
    connect(dirUpAct, SIGNAL(triggered()), this, SLOT(dirUp()));

    QCompleter *completer = new QCompleter(this);
    completer->setModel(fsm);
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
            treeViewFilesystem->setCurrentIndex(fsm->index(path));
        }
    }
}

void KomicViewer::closeEvent(QCloseEvent *event)
{
    Settings::Instance()->setLastPath(fsm->filePath(treeViewFilesystem->currentIndex()));
}

void KomicViewer::keyPressEvent(QKeyEvent *event)
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

void KomicViewer::createActions()
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

void KomicViewer::createMenus(QMenuBar *parent)
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
    this->addAction(toggleFullscreenAct);
    optionsMenu->addAction(toggleFullscreenAct);
    optionsMenu->addAction(togglePanelAct);
    optionsMenu->addAction(largeIconsAct);
    optionsMenu->addAction(settingsAct);

    parent->addMenu(fileMenu);
    parent->addMenu(editMenu);
    parent->addMenu(optionsMenu);
}


void KomicViewer::connectActions()
{
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    connect(settingsAct, SIGNAL(triggered()), this, SLOT(settingsDialog()));
    connect(pagePreviousAct, SIGNAL(triggered()), this, SLOT(pagePrevious()));
    connect(pageNextAct, SIGNAL(triggered()), this, SLOT(pageNext()));
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

void KomicViewer::openFile(const QString &source)
{
    treeViewFilesystem->setCurrentIndex(fsm->index(source));
}

bool KomicViewer::checkFileExtension(const QFileInfo &fi)
{
    if(filters_archive.contains(fi.suffix().toLower()) || fi.isDir() == true)
    {
       return true;
    }
    else
    {
        return false;
    }
}

bool KomicViewer::acceptFileDrop(const QMimeData* mimeData){
     if (mimeData->hasUrls())
     {
         return checkFileExtension(QFileInfo(mimeData->urls().at(0).toLocalFile()));
     }
     else
     {
         return false;
     }
}

void KomicViewer::dragEnterEvent(QDragEnterEvent* event)
{
    if(acceptFileDrop(event->mimeData()) == true)
    {
        event->acceptProposedAction();
    }
}

void KomicViewer::dragMoveEvent(QDragMoveEvent* event)
{
    // if some actions should not be usable, like move, this code must be adopted
    event->acceptProposedAction();
}


void KomicViewer::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void KomicViewer::dropEvent(QDropEvent* event)
{
    if(event->proposedAction() == Qt::CopyAction)
    {
        openFile(event->mimeData()->urls().at(0).toLocalFile());
    }
    event->acceptProposedAction();
}

void KomicViewer::OnPathEdited()
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


void KomicViewer::OnTreeViewItemActivated ( const QModelIndex & index )
{
    if(lineEditPath->palette() != QApplication::palette())
    {
        lineEditPath->setPalette(QApplication::palette());

	updatePath(fsm->filePath(index));
    }
}

void KomicViewer::updatePath(const QString &filePath)
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

void KomicViewer::refreshPath()
{
    OnTreeViewCurrentChanged(treeViewFilesystem->currentIndex(), treeViewFilesystem->currentIndex());
}

void KomicViewer::OnTreeViewCurrentChanged(const QModelIndex & current, const QModelIndex & previous)
{
    dirUpAct->setEnabled(current.parent().isValid());

    treeViewFilesystem->scrollTo(current);

    QString filePath = fsm->filePath(current);

    setWindowTitle(fsm->fileName(current) + " - " + QApplication::applicationName() + " " + QApplication::applicationVersion());

    updatePath(filePath);
    if(fsm->isDir(current))
    {
	fsm->fetchMore(current);

        QDir dir(filePath);

//        dir.setNameFilters(imagefilter);
        QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
        QList<QTreeWidgetItem*> items;
        QFileIconProvider fip;
        for (int i=0; i < list.count(); i++)
        {
            QTreeWidgetItem* ntvfi = NULL;

            QFileInfo info = list.at(i);
            if(info.isDir())
            {
                ntvfi = new QTreeWidgetItem(LV_TYPE_DIR);
            }
            else if(filters_archive.contains(info.suffix().toLower()))
            {
                ntvfi = new QTreeWidgetItem(LV_TYPE_ARCHIVE);
            }
            else if (filters_image.contains(info.suffix().toLower()))
            {
                ntvfi = new QTreeWidgetItem(LV_TYPE_FILE);
            }

            if(ntvfi != NULL)
            {
                ntvfi->setText(LV_COLNAME, info.fileName());
                ntvfi->setIcon(LV_COLNAME, fip.icon(info));
                items << ntvfi;
            }

        }
        treeWidgetFiles->clear();
        treeWidgetFiles->setRootIsDecorated(false);
        treeWidgetFiles->addTopLevelItems(items);

        for(int i = 0; i < treeWidgetFiles->header()->count(); i++)
        {
            treeWidgetFiles->resizeColumnToContents(i);
        }

	archive_files.clear();
    }
    else
    {
        // Read info from zip file

        QFile zipFile(filePath);
        QuaZip zip(&zipFile);
        if(!zip.open(QuaZip::mdUnzip))
        {
            qWarning("testRead(): zip.open(): %d", zip.getZipError());
            return;
        }
        zip.setFileNameCodec("UTF-8");

        archive_files = zip.getFileInfoList();

        zip.close();
        if(zip.getZipError()!=UNZ_OK) {
            qWarning("testRead(): zip.close(): %d", zip.getZipError());
            return;
        }


        //Populate treeViewFile


        QTreeWidgetItem* root = new QTreeWidgetItem(LV_TYPE_ARCHIVE);
        QTreeWidgetItem* node = root;
        QFileIconProvider fip;
        root->setIcon(LV_COLNAME, fip.icon(QFileInfo(zipFile)));
	root->setText(LV_COLNAME, fsm->fileInfo(current).fileName());

        for(int i=0; i < archive_files.count() ; i++)
        {
            node = root;
            QStringList file_path_parts = archive_files.at(i).name.split('/');
            for (int j = 0; j < file_path_parts.count(); j++)
            {
                if (file_path_parts.at(j).count() > 0)
                {
                    if (j < file_path_parts.count() - 1)
                    {
                        node = AddNode(node, file_path_parts.at(j), LV_TYPE_DIR);
                    }
                    else
                        //if (j == file_path_parts.count() - 1)
                    {
                        QFileInfo fi(archive_files.at(i).name);
                        if(filters_image.contains(fi.suffix().toLower()))
                        {
//                            qDebug() << fi.completeBaseName() << fi.suffix();
                            node = AddNode(node, file_path_parts.at(j), makeArchiveNumberForTreewidget(i));
                        }
                    }
                }
            }
        }

        treeWidgetFiles->clear();
	treeWidgetFiles->setRootIsDecorated(true);
        treeWidgetFiles->addTopLevelItems(root->takeChildren());
    }
}

QTreeWidgetItem* KomicViewer::AddNode(QTreeWidgetItem* node, QString name, int index)
{
    for(int i = 0; i < node->childCount(); i++)
    {
        if(node->child(i)->text(LV_COLNAME) == name)
        {
            return node->child(i);
        }
    }

    QTreeWidgetItem* ntvi = new QTreeWidgetItem(index);
    ntvi->setText(LV_COLNAME, name);
    if(index == LV_TYPE_DIR) ntvi->setIcon(LV_COLNAME, System_icons::getDirectoryIcon());
    node->addChild(ntvi);
    return ntvi;
}

void KomicViewer::togglePanel(bool value)
{
    splitterPanel->setVisible(value);
    toolbarFiles->setVisible(value);
//    refreshPathAct->setVisible(value);
//    dirUpAct->setVisible(value);
//    lineEditPath->setEnabled(value);
//    lineEditPathAction->setVisible(value);

}

void KomicViewer::toggleFullscreen(bool value)
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

void KomicViewer::OnTreeFileWidgetItemActivated ( QTreeWidgetItem * item, int column )
{
    if(fsm->isDir(treeViewFilesystem->currentIndex()))
    {
        if(item->type() == LV_TYPE_DIR || item->type() == LV_TYPE_ARCHIVE)
	{
	    treeViewFilesystem->setCurrentIndex(fsm->index(fsm->filePath(treeViewFilesystem->currentIndex()) + "/" + item->text(LV_COLNAME)));

//	    for(int i = 0; i < fsm->rowCount(treeViewFilesystem->currentIndex()); i++)
//            {
//		if(fsm->index(i, 0, treeViewFilesystem->currentIndex()).data().toString() == item->text(LV_COLNAME))
//                {
//		    treeViewFilesystem->setCurrentIndex(fsm->index(i,0, treeViewFilesystem->currentIndex()));
//                    break;
//                }
//            }

            treeViewFilesystem->expand(treeViewFilesystem->currentIndex());
        }
    }
}

void KomicViewer::dirUp()
{
    if(treeViewFilesystem->currentIndex().parent().isValid())
    {
	treeViewFilesystem->setCurrentIndex(treeViewFilesystem->currentIndex().parent());
    }
}

void KomicViewer::OnTreeFileWidgetCurrentChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
    if(current == NULL || current->type() == LV_TYPE_DIR || current->type() == LV_TYPE_ARCHIVE)
    {
        imageDisplay->setPixmap(NULL);
    }
    else
    {

	QString filepath = fsm->filePath(treeViewFilesystem->currentIndex());

	if(fsm->isDir(treeViewFilesystem->currentIndex()))
        {
            QString path = filepath + "/" + current->text(LV_COLNAME);

            imageDisplay->setPixmap(QPixmap(path));
        }
        else
        {
            int SelectedZipFileIndex = getArchiveNumberFromTreewidget(treeWidgetFiles->currentItem()->type());

            if(SelectedZipFileIndex < 0)
            {
                imageDisplay->setPixmap(NULL);
            }
            else
            {
                QFile zipFile(filepath);
                QuaZip zip(&zipFile);
                if(!zip.open(QuaZip::mdUnzip))
                {
                    qWarning("testRead(): zip.open(): %d", zip.getZipError());
                    return;
                }
                zip.setFileNameCodec("UTF-8");
                zip.setCurrentFile(archive_files.at(SelectedZipFileIndex).name);

                QuaZipFile file(&zip);
                char c;
                if(!file.open(QIODevice::ReadOnly))
                {
                    qWarning("testRead(): file.open(): %d", file.getZipError());
                    return;
                }

                QBuffer out;
                out.open(QIODevice::WriteOnly);
                char buf[4096];
                int len = 0;
                while (file.getChar(&c))
                {
                    buf[len++] = c;
                    if (len >= 4096)
                    {
                        out.write(buf, len);
                        len = 0;
                    }
                }
                if (len > 0)
                {
                    out.write(buf, len);
                }
                out.close();

		QPixmap pm;
		pm.loadFromData(out.buffer());
                imageDisplay->setPixmap(pm);
            }
        }
    }

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

void KomicViewer::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Zip files (*.zip *.cbz)"));
    if (!fileName.isEmpty())
    {
        openFile(fileName);
    }
}

bool KomicViewer::saveAs()
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

void KomicViewer::zoomReset()
{
    imageDisplay->setZoom(1);
}

void KomicViewer::lockZoom()
{
    imageDisplay->setLockMode(LockMode::Zoom);
}

void KomicViewer::lockNone()
{
    imageDisplay->setLockMode(LockMode::None);
}

void KomicViewer::lockAutofit()
{
    imageDisplay->setLockMode(LockMode::Autofit);
}

void KomicViewer::lockFitWidth()
{
    imageDisplay->setLockMode(LockMode::FitWidth);
}

void KomicViewer::lockFitHeight()
{
    imageDisplay->setLockMode(LockMode::FitHeight);
}

void KomicViewer::rotateLeft()
{
    imageDisplay->setRotation(imageDisplay->getRotation()-10);
}

void KomicViewer::rotateRight()
{
    imageDisplay->setRotation(imageDisplay->getRotation()+10);
}

void KomicViewer::rotateReset()
{
    imageDisplay->setRotation(0);
}

void KomicViewer::settingsDialog()
{
    Settings_Dialog sd(this);
    if(sd.exec() == QDialog::Accepted)
    {
        //update settings
    }
}

void KomicViewer::toggleLargeIcons(bool value)
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

void KomicViewer::pageNext()
{
    if(treeWidgetFiles->currentItem() == NULL) return;

    if(treeWidgetFiles->currentItem()->parent() == NULL)
    {
        int curr = treeWidgetFiles->indexOfTopLevelItem(treeWidgetFiles->currentItem());
        int max = treeWidgetFiles->topLevelItemCount();

        for(int i = curr + 1; i < max; i++)
        {
            if(treeWidgetFiles->topLevelItem(i)->type() == LV_TYPE_FILE)
            {
                treeWidgetFiles->setCurrentItem(treeWidgetFiles->topLevelItem(i));
                break;
            }
        }

    }
    else
    {
        QTreeWidgetItem* ctwip = treeWidgetFiles->currentItem()->parent();
        int curr = ctwip->indexOfChild(treeWidgetFiles->currentItem());
        int max = ctwip->childCount();

        for(int i = curr + 1; i < max; i++)
        {
            if(ctwip->child(i)->type() != LV_TYPE_DIR)
            {
                treeWidgetFiles->setCurrentItem(ctwip->child(i));
                break;
            }
        }
    }
}

void KomicViewer::pagePrevious()
{
    if(treeWidgetFiles->currentItem() == NULL) return;
    if(treeWidgetFiles->currentItem()->parent() == NULL)
    {
        int curr = treeWidgetFiles->indexOfTopLevelItem(treeWidgetFiles->currentItem());

        for(int i = curr - 1; i >= 0; i--){
            if(treeWidgetFiles->topLevelItem(i)->type() == LV_TYPE_FILE) {
                treeWidgetFiles->setCurrentItem(treeWidgetFiles->topLevelItem(i));
                break;
            }
        }

    }
    else
    {
        QTreeWidgetItem* ctwip = treeWidgetFiles->currentItem()->parent();
        int curr = ctwip->indexOfChild(treeWidgetFiles->currentItem());

        for(int i = curr - 1; i >= 0; i--){
            if(ctwip->child(i)->type() != LV_TYPE_DIR) {
                treeWidgetFiles->setCurrentItem(ctwip->child(i));
                break;
            }
        }
    }
}

void KomicViewer::OnZoomChanged()
{
    QString zoomText = QString::number((int)(imageDisplay->getZoom() * 100)) + "%";
    comboBoxZoom->setEditText(zoomText);
}

bool KomicViewer::parseZoom(const QString &zoomText)
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

void KomicViewer::OnComboBoxZoomTextChanged()
{
    if(parseZoom(comboBoxZoom->lineEdit()->text()) == false)
    {
        OnZoomChanged();
    }
}

void KomicViewer::OnComboBoxZoomIndexChanged(const int &index)
{
    imageDisplay->setZoom(imageDisplay->getDefaultZoomSizes().at(index));
}

int getArchiveNumberFromTreewidget(int number)
{
    return number - 2000;
}

int makeArchiveNumberForTreewidget(int number)
{
    return number + 2000;
}
