#include "main_window.h"
#include "settings_dialog.h"
#include "settings.h"
#include "quazip/JlCompress.h"

#include <QUrl>
#include <QAction>
#include <QBoxLayout>
#include <QFileDialog>
#include <QHeaderView>
#include <QDesktopWidget>
#include <QCompleter>
#include <QMessageBox>
#include <QDesktopServices>
#include <QKeyEvent>
#include <QInputDialog>

#define DEBUG_MAIN_WINDOW_ICONS

//#define DEBUG_MAIN_WINDOW
#ifdef DEBUG_MAIN_WINDOW
#include <QDebug>
#endif

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent, f)
    , m_flag_opening(false)

    , m_settings(new Settings())

    , m_model_filesystem(new QFileSystemModel(this))
    , m_view_filesystem(new QTreeView(this))
    , m_view_files(new ViewFiles(this))

    , m_splitter_main(new QSplitter(Qt::Horizontal, this))
    , m_splitter_sidebar(new QSplitter(Qt::Vertical, this))
    , m_picture_item(new PictureItem(m_settings, this))

    , m_menu_main(new QMenuBar(this))
    , m_toolbar(new QToolBar(this))
    , m_lineEdit_path(new QLineEdit(this))
    , m_comboBox_zoom(new QComboBox(this))
{
    this->setAcceptDrops(true);

    if (Helper::getFiltersImage().contains("svg"))
    {
        this->setWindowIcon(QIcon(":/icons/kiv.svg"));
    }

    /* Start modelFilesystem */
    QStringList filters;
    QStringList filtersArchive = Helper::getFiltersArchive();
    for (int i = 0; i < filtersArchive.size(); ++i)
    {
        filters.append("*." + filtersArchive.at(i));
    }

    m_model_filesystem->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    m_model_filesystem->setNameFilterDisables(false);
    m_model_filesystem->setNameFilters(filters);
    m_model_filesystem->setRootPath("");
    /* End modelFilesystem */

    /* Start filesystemView */
    m_view_filesystem->setUniformRowHeights(true);
    m_view_filesystem->setHeaderHidden(true);
    m_view_filesystem->setModel(m_model_filesystem);
    for (int i = 1; i < m_view_filesystem->header()->count(); ++i)
    {
        m_view_filesystem->hideColumn(i);
    }
    /* End filesystemView */

    m_view_files->setThumbnailsSize(m_settings->getThumbnailSize());


    createActions();
    createMenus();

    QCompleter *completer = new QCompleter(this);
    completer->setModel(m_model_filesystem);
    m_lineEdit_path->setCompleter(completer);
    m_lineEdit_path->installEventFilter(this);

    /* Start comboBoxZoom */
    m_comboBox_zoom->setEnabled(false);
    m_comboBox_zoom->setInsertPolicy(QComboBox::NoInsert);
    m_comboBox_zoom->setMaxVisibleItems(12);
    m_comboBox_zoom->setMinimumContentsLength(8);
    m_comboBox_zoom->setEditable(true);
    m_comboBox_zoom->setFocusPolicy(Qt::ClickFocus);
    m_comboBox_zoom->lineEdit()->setCompleter(0);
    m_comboBox_zoom->installEventFilter(this);

    for (int i = 0; i < m_picture_item->getDefaultZoomSizes().size(); ++i)
    {
        const qreal &z = m_picture_item->getDefaultZoomSizes().at(i);
        m_comboBox_zoom->addItem(QString::number((z * 100), 'f', 0) + "%", z);
        if (z == 1)
        {
            m_comboBox_zoom->setCurrentIndex(m_comboBox_zoom->count() - 1);
        }
    }
    /* End comboBoxZoom */


    /* Large icons are On by default but I want small icons by default */
    if (m_settings->getLargeIcons())
    {
        m_act_largeIcons->setChecked(true);
    }
    else
    {
        toggleLargeIcons(false);
    }

    connectActions();

    /* Start imageDisplay */
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    policy.setHorizontalStretch(1);
    policy.setVerticalStretch(0);
    m_picture_item->setSizePolicy(policy);
    /* End imageDisplay */

    this->setMenuBar(m_menu_main);

    QWidget *content = new QWidget(this);
    QVBoxLayout *vboxMain = new QVBoxLayout(content);
    vboxMain->setSpacing(0);
    vboxMain->setMargin(0);

    m_splitter_sidebar->addWidget(m_view_filesystem);
    m_splitter_sidebar->addWidget(m_view_files);

    m_splitter_main->addWidget(m_splitter_sidebar);
    m_splitter_main->setSizes(QList<int>() << 300);
    m_splitter_main->addWidget(m_picture_item);

    vboxMain->addWidget(m_toolbar);
    vboxMain->addWidget(m_splitter_main);

    this->setCentralWidget(content);


    if (QApplication::arguments().size() > 1)
    {
        this->openFile(QApplication::arguments().at(1));
    }
    else
    {
        this->openFile(m_settings->getLastPath());
    }

    m_picture_item->setFocus();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_settings->setLastPath(m_view_files->getCurrentFileInfo().getPath());
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
#ifdef DEBUG_MAIN_WINDOW
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::keyPressEvent" << event->key();
#endif
    if (event->key() == Qt::Key_Escape)
    {
        if (m_lineEdit_path->hasFocus())
        {
            m_lineEdit_path->setText(m_view_files->getCurrentFileInfo().getPath());
            m_lineEdit_path->clearFocus();
            event->accept();
        }
        else if (m_comboBox_zoom->lineEdit()->hasFocus())
        {
            on_zoom_changed(m_picture_item->getZoom(), m_picture_item->getZoom());
            m_comboBox_zoom->clearFocus();
            event->accept();
        }
    }
    else if (event->key() == Qt::Key_B)
    {
        showMinimized();
        event->accept();
    }
}

void MainWindow::createActions()
{
    QDir appdir(QApplication::applicationDirPath());
#ifdef DEBUG_MAIN_WINDOW_ICONS
    if (appdir.dirName() == "release" || appdir.dirName() == "debug")
    {
        appdir.cd("../../src/icons");
    }
#endif
    QIcon::setThemeSearchPaths(QStringList(QIcon::themeSearchPaths()) << appdir.path());

    static const char * GENERIC_ICON_TO_CHECK = "media-skip-backward";
    static const char * FALLBACK_ICON_THEME = "default";
    if (!QIcon::hasThemeIcon(GENERIC_ICON_TO_CHECK)) {
        /* If there is no default working icon theme then we should
           use an icon theme that we provide via a icons folder
           This case happens under Windows and Mac OS X
           This does not happen under GNOME or KDE */
        QIcon::setThemeName(FALLBACK_ICON_THEME);
    }


    /* File Actions */

    m_act_open = new QAction(QIcon::fromTheme("document-open"), tr("&Open..."), this);
    m_act_open->setShortcut(tr("Ctrl+O"));

    m_act_save = new QAction(QIcon::fromTheme("document-save-as"), tr("&Save Page As..."), this);
    m_act_save->setShortcut(tr("Ctrl+S"));
    m_act_save->setEnabled(false);

    //    printAct = new QAction(tr("&Print..."), this);
    //    printAct->setShortcut(tr("Ctrl+P"));
    //    printAct->setEnabled(false);
    //    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    m_act_exit = new QAction(QIcon::fromTheme("application-exit"), tr("E&xit"), this);
    m_act_exit->setShortcut(tr("Ctrl+Q"));

    m_act_bookmark_add = new QAction(tr("Bookmark &This Page"), this);
    m_act_bookmark_add->setShortcut(tr("Ctrl+D"));


    /* Options Actions */

    m_act_pagePrevious = new QAction(QIcon::fromTheme("go-previous"), tr("&Previous Page"), this);
    m_act_pagePrevious->setShortcut(Qt::Key_PageUp);

    m_act_pageNext = new QAction(QIcon::fromTheme("go-next"), tr("&Next Page"), this);
    m_act_pageNext->setShortcut(Qt::Key_PageDown);

    m_act_rotateLeft = new QAction(QIcon::fromTheme("object-rotate-left"), tr("Rotate &Left"), this);
    m_act_rotateLeft->setEnabled(false);

    m_act_rotateRight = new QAction(QIcon::fromTheme("object-rotate-right"), tr("Rotate &Right"), this);
    m_act_rotateRight->setEnabled(false);

    m_act_rotateReset = new QAction(tr("R&eset Rotation"), this);
    m_act_rotateReset->setEnabled(false);

    m_act_thumbnails = new QAction(QIcon::fromTheme("image-x-generic"), tr("&Show Thumbnails"), this);
    m_act_thumbnails->setCheckable(true);

    m_act_mode_icons = new QAction(tr("&Icons"), this);
    m_act_mode_icons->setCheckable(true);

    m_act_mode_details = new QAction(tr("&Details"), this);
    m_act_mode_details->setCheckable(true);

    m_act_mode_list = new QAction(tr("&List"), this);
    m_act_mode_list->setCheckable(true);
    m_act_mode_list->setChecked(true);


    m_act_fullscreen = new QAction(QIcon::fromTheme("view-fullscreen"), tr("&Full Screen"), this);
    m_act_fullscreen->setShortcut(Qt::Key_F11);
    m_act_fullscreen->setCheckable(true);

    m_act_sidebar = new QAction(QIcon::fromTheme("view-split-left-right"),tr("Show Side&bar"), this);
    m_act_sidebar->setCheckable(true);
    m_act_sidebar->setChecked(true);

    m_act_largeIcons = new QAction(tr("Large Toolbar &Icons"), this);
    m_act_largeIcons->setCheckable(true);

    m_act_settings = new QAction(QIcon::fromTheme("configure", QIcon::fromTheme("gtk-preferences")), tr("&Settings..."), this);
    m_act_settings->setMenuRole(QAction::PreferencesRole);
    //    settingsAct->setVisible(false);


    /* Zoom Actions */

    m_act_zoomIn = new QAction(QIcon::fromTheme("zoom-in"), tr("Zoom &In"), this);
    m_act_zoomIn->setShortcut(Qt::CTRL | Qt::Key_Plus);
    m_act_zoomIn->setEnabled(false);

    m_act_zoomOut = new QAction(QIcon::fromTheme("zoom-out"), tr("Zoom &Out"), this);
    m_act_zoomOut->setShortcut(Qt::CTRL | Qt::Key_Minus);
    m_act_zoomOut->setEnabled(false);

    m_act_zoomReset = new QAction(QIcon::fromTheme("zoom-original"), tr("&Original Size (100%)"), this);
    m_act_zoomReset->setShortcut(Qt::CTRL | Qt::Key_0);
    m_act_zoomReset->setEnabled(false);

    m_act_fitToWindow = new QAction(QIcon::fromTheme("zoom-fit-best"), tr("&Fit to Window"), this);
    m_act_fitToWindow->setShortcut(Qt::CTRL | Qt::Key_T);
    m_act_fitToWindow->setEnabled(false);

    m_act_fitToWidth = new QAction(QIcon::fromTheme("zoom-fit-width"), tr("Fit &Width"), this);
    m_act_fitToWidth->setEnabled(false);

    m_act_fitToHeight = new QAction(QIcon::fromTheme("zoom-fit-height"), tr("Fit &Height"), this);
    m_act_fitToHeight->setEnabled(false);

    m_act_lockNone = new QAction(tr("Lock &None"), this);
    m_act_lockNone->setCheckable(true);
    m_act_lockNone->setChecked(true);

    m_act_lockAutofit = new QAction(tr("Lock Autofi&t"), this);
    m_act_lockAutofit->setCheckable(true);

    m_act_lockFitHeight = new QAction(tr("Lock Fit Hei&ght"), this);
    m_act_lockFitHeight->setCheckable(true);

    m_act_lockFitWidth = new QAction(tr("Lock Fit Wi&dth"), this);
    m_act_lockFitWidth->setCheckable(true);

    m_act_lockZoom = new QAction(tr("Lock Zoom &Value"), this);
    m_act_lockZoom->setCheckable(true);


    /* Help Actions */

    m_act_webSite = new QAction(tr("&Web Site"), this);

    m_act_about = new QAction(tr("&About"), this);
    m_act_about->setShortcut(Qt::Key_F1);


    /* Toolbar Actions */

    m_act_refreshPath = new QAction(QIcon::fromTheme("view-refresh"), tr("&Refresh"), this);

    m_act_dirUp = new QAction(QIcon::fromTheme("go-up"), tr("Go &Up"), this);
    m_act_dirUp->setEnabled(false);

    m_act_bookmark_delete = new QAction(tr("&Delete Bookmark"), this);

}

void MainWindow::createMenus()
{
    QMenu *fileMenu = m_menu_main->addMenu(tr("&File"));
    fileMenu->addAction(m_act_open);
    fileMenu->addAction(m_act_save);

    m_menu_bookmarks = fileMenu->addMenu(tr("&Bookmarks"));
    m_menu_bookmarks->addAction(m_act_bookmark_add);
    m_menu_bookmarks->addSeparator();

    m_menu_bookmarks->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_menu_bookmarks, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_bookmark_customContextMenuRequested(QPoint)));

    populateBookmarks();

    fileMenu->addAction(m_act_exit);

    QMenu *editMenu = m_menu_main->addMenu(tr("&Edit"));
    editMenu->addAction(m_act_zoomIn);
    this->addAction(m_act_zoomIn);
    editMenu->addAction(m_act_zoomOut);
    this->addAction(m_act_zoomOut);
    editMenu->addAction(m_act_zoomReset);
    this->addAction(m_act_zoomReset);
    editMenu->addSeparator();
    editMenu->addAction(m_act_rotateLeft);
    editMenu->addAction(m_act_rotateRight);
    editMenu->addAction(m_act_rotateReset);
    editMenu->addSeparator();
    editMenu->addAction(m_act_fitToWindow);
    this->addAction(m_act_fitToWindow);
    editMenu->addAction(m_act_fitToWidth);
    editMenu->addAction(m_act_fitToHeight);
    editMenu->addSeparator();

    QActionGroup *lockActions = new QActionGroup(this);
    lockActions->addAction(m_act_lockNone);
    lockActions->addAction(m_act_lockZoom);
    lockActions->addAction(m_act_lockAutofit);
    lockActions->addAction(m_act_lockFitWidth);
    lockActions->addAction(m_act_lockFitHeight);

    editMenu->addAction(m_act_lockNone);
    editMenu->addAction(m_act_lockZoom);
    editMenu->addAction(m_act_lockAutofit);
    editMenu->addAction(m_act_lockFitWidth);
    editMenu->addAction(m_act_lockFitHeight);


    QMenu *optionsMenu = m_menu_main->addMenu(tr("&Options"));
    this->addAction(m_act_pagePrevious);
    this->addAction(m_act_pageNext);
    optionsMenu->addAction(m_act_pagePrevious);
    optionsMenu->addAction(m_act_pageNext);
    optionsMenu->addSeparator();
    optionsMenu->addAction(m_act_thumbnails);

    QActionGroup *viewModes = new QActionGroup(this);
    viewModes->addAction(m_act_mode_list);
    viewModes->addAction(m_act_mode_details);
    viewModes->addAction(m_act_mode_icons);

    QMenu *menuViewMode = optionsMenu->addMenu("&View Mode");
    menuViewMode->addAction(m_act_mode_list);
    menuViewMode->addAction(m_act_mode_details);
    menuViewMode->addAction(m_act_mode_icons);

    this->addAction(m_act_fullscreen);
    optionsMenu->addAction(m_act_fullscreen);
    optionsMenu->addAction(m_act_sidebar);
    optionsMenu->addAction(m_act_largeIcons);
    optionsMenu->addAction(m_act_settings);

    QMenu *helpMenu = m_menu_main->addMenu(tr("&Help"));
    helpMenu->addAction(m_act_webSite);
    helpMenu->addAction(m_act_about);


    /* Start toolbar */
    m_toolbar->setMovable(false);
    m_toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    m_toolbar->layout()->setMargin(0);

    m_toolbar->addAction(m_act_sidebar);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_act_dirUp);
    m_toolbar->addAction(m_act_refreshPath);
    m_toolbar->addWidget(m_lineEdit_path);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_act_pagePrevious);
    m_toolbar->addAction(m_act_pageNext);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_act_zoomIn);
    m_toolbar->addAction(m_act_zoomOut);
    m_toolbar->addAction(m_act_fitToWindow);
    m_toolbar->addWidget(m_comboBox_zoom);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_act_rotateLeft);
    m_toolbar->addAction(m_act_rotateRight);
    m_toolbar->addAction(m_act_fullscreen);
    /* End toolbar */


    /* Start contextMenu */
    m_menu_context_picture = new QMenu(this);
    m_menu_context_picture->addAction(m_act_pageNext);
    m_menu_context_picture->addAction(m_act_pagePrevious);
    m_menu_context_picture->addSeparator();
    m_menu_context_picture->addAction(m_act_fullscreen);
    m_menu_context_picture->addAction(m_act_sidebar);
    m_menu_context_picture->addSeparator();

    QMenu *menuZoom = m_menu_context_picture->addMenu(tr("Zoom"));
    menuZoom->addAction(m_act_zoomIn);
    menuZoom->addAction(m_act_zoomOut);
    menuZoom->addAction(m_act_zoomReset);

    QMenu *menuRotate = m_menu_context_picture->addMenu(tr("Rotate"));
    menuRotate->addAction(m_act_rotateLeft);
    menuRotate->addAction(m_act_rotateRight);
    menuRotate->addAction(m_act_rotateReset);

    QMenu *menuFit = m_menu_context_picture->addMenu(tr("Fit"));
    menuFit->addAction(m_act_fitToHeight);
    menuFit->addAction(m_act_fitToWidth);
    menuFit->addAction(m_act_fitToWindow);

    QMenu *menuLock = m_menu_context_picture->addMenu(tr("Lock"));
    menuLock->addAction(m_act_lockNone);
    menuLock->addAction(m_act_lockZoom);
    menuLock->addAction(m_act_lockAutofit);
    menuLock->addAction(m_act_lockFitWidth);
    menuLock->addAction(m_act_lockFitHeight);

    m_menu_context_picture->addSeparator();
    m_menu_context_picture->addAction(m_act_exit);


    /* End contextMenu */

    m_menu_context_bookmark = new QMenu(this);
    m_menu_context_bookmark->addAction(m_act_bookmark_delete);
}

void MainWindow::connectActions()
{
    connect(m_act_open, SIGNAL(triggered()), this, SLOT(open()));
    connect(m_act_save, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(m_act_exit, SIGNAL(triggered()), this, SLOT(close()));

    connect(m_act_bookmark_add, SIGNAL(triggered()), this, SLOT(addBookmark()));

    connect(m_act_settings, SIGNAL(triggered()), this, SLOT(settingsDialog()));
    connect(m_act_pagePrevious, SIGNAL(triggered()), m_view_files, SLOT(pagePrevious()));
    connect(m_act_pageNext, SIGNAL(triggered()), m_view_files, SLOT(pageNext()));


    connect(m_act_thumbnails, SIGNAL(toggled(bool)), this, SLOT(toggleShowThumbnails(bool)));
    connect(m_act_fullscreen, SIGNAL(toggled(bool)), this, SLOT(toggleFullscreen(bool)));
    connect(m_act_largeIcons, SIGNAL(toggled(bool)), this, SLOT(toggleLargeIcons(bool)));
    connect(m_act_sidebar, SIGNAL(toggled(bool)), this, SLOT(toggleSidebar(bool)));
    connect(m_act_mode_list, SIGNAL(triggered()), this, SLOT(on_view_mode_list_triggered()));
    connect(m_act_mode_details, SIGNAL(triggered()), this, SLOT(on_view_mode_details_triggered()));
    connect(m_act_mode_icons, SIGNAL(triggered()), this, SLOT(on_view_mode_icons_triggered()));

    connect(m_act_rotateLeft, SIGNAL(triggered()), this, SLOT(rotateLeft()));
    connect(m_act_rotateRight, SIGNAL(triggered()), this, SLOT(rotateRight()));
    connect(m_act_rotateReset, SIGNAL(triggered()), this, SLOT(rotateReset()));
    connect(m_act_zoomIn, SIGNAL(triggered()), m_picture_item, SLOT(zoomIn()));
    connect(m_act_zoomOut, SIGNAL(triggered()), m_picture_item, SLOT(zoomOut()));
    connect(m_act_zoomReset, SIGNAL(triggered()), this, SLOT(zoomReset()));
    connect(m_act_fitToWindow, SIGNAL(triggered()), m_picture_item, SLOT(fitToScreen()));
    connect(m_act_fitToWidth, SIGNAL(triggered()), m_picture_item, SLOT(fitWidth()));
    connect(m_act_fitToHeight, SIGNAL(triggered()), m_picture_item, SLOT(fitHeight()));

    connect(m_act_lockNone, SIGNAL(triggered()), this, SLOT(lockNone()));
    connect(m_act_lockZoom, SIGNAL(triggered()), this, SLOT(lockZoom()));
    connect(m_act_lockAutofit, SIGNAL(triggered()), this, SLOT(lockAutofit()));
    connect(m_act_lockFitHeight, SIGNAL(triggered()), this, SLOT(lockFitHeight()));
    connect(m_act_lockFitWidth, SIGNAL(triggered()), this, SLOT(lockFitWidth()));

    connect(m_act_about, SIGNAL(triggered()), this, SLOT(about()));
    connect(m_act_webSite, SIGNAL(triggered()), this, SLOT(website()));

    connect(m_view_files, SIGNAL(activated(QString)), this, SLOT(on_filesView_item_activated(QString)));
    connect(m_lineEdit_path, SIGNAL(returnPressed()), this, SLOT(on_lineEditPath_editingFinished()));
    connect(m_view_filesystem->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(on_filesystemView_currentRowChanged(QModelIndex,QModelIndex)));
    connect(m_view_files, SIGNAL(currentFileChanged(FileInfo)), this, SLOT(on_filesView_currentChanged(FileInfo)));

    connect(m_picture_item, SIGNAL(imageChanged()), this, SLOT(updateActions()));
    connect(m_picture_item, SIGNAL(toggleFullscreen()), m_act_fullscreen, SLOT(toggle()));
    connect(m_picture_item, SIGNAL(pageNext()), m_view_files, SLOT(pageNext()));
    connect(m_picture_item, SIGNAL(pagePrevious()), m_view_files, SLOT(pagePrevious()));
    connect(m_picture_item, SIGNAL(zoomChanged(qreal,qreal)), this, SLOT(on_zoom_changed(qreal,qreal)));
    connect(m_picture_item, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_customContextMenuRequested(QPoint)));
    connect(m_picture_item, SIGNAL(quit()), this, SLOT(close()));
    connect(m_picture_item, SIGNAL(boss()), this, SLOT(showMinimized()));
    connect(m_picture_item, SIGNAL(setFullscreen(bool)), m_act_fullscreen, SLOT(setChecked(bool)));

    connect(m_comboBox_zoom, SIGNAL(activated(int)), this, SLOT(on_comboBoxZoom_activated(int)));
    connect(m_comboBox_zoom->lineEdit(), SIGNAL(returnPressed()), this, SLOT(on_comboBoxZoom_TextChanged()));

    connect(m_act_refreshPath, SIGNAL(triggered()), this, SLOT(refreshPath()));
    connect(m_act_dirUp, SIGNAL(triggered()), this, SLOT(dirUp()));

    connect(m_act_bookmark_delete, SIGNAL(triggered()), this, SLOT(deleteBookmark()));

}

void MainWindow::populateBookmarks()
{
    QList<QAction*> oldActions = m_menu_bookmarks->actions();

    for (int i = 0; i < oldActions.size(); ++i)
    {
        if (!oldActions.at(i)->data().toString().isEmpty())
        {
#ifdef DEBUG_MAIN_WINDOW
                qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::populateBookmarks()" << "removed bookmark" << oldActions.at(i)->text() << oldActions.at(i)->data().toString();
#endif
            oldActions.at(i)->deleteLater();
        }
    }

    QList<Bookmark> bookmarks = m_settings->getBookmarks();
    for (int i = 0; i < bookmarks.size(); ++i)
    {
        QAction *bookmark = new QAction(bookmarks.at(i).getName(), this);
        bookmark->setData(i);
        connect(bookmark, SIGNAL(triggered()), this, SLOT(on_bookmark_triggered()));
        m_menu_bookmarks->addAction(bookmark);
#ifdef DEBUG_MAIN_WINDOW
                qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::populateBookmarks()" << "added bookmark" << bookmark->text() << bookmark->data().toString();
#endif
    }
}

void MainWindow::on_customContextMenuRequested(const QPoint &pos)
{
    m_menu_context_picture->popup(m_picture_item->mapToGlobal(pos));
}

void MainWindow::on_bookmark_triggered()
{
    if (QAction *action = qobject_cast<QAction *>(sender()))
    {
        if (action->data().isNull())
            return;
        int bookmarkIndex = action->data().toInt();
#ifdef DEBUG_MAIN_WINDOW
                qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::on_bookmark_triggered()";
#endif
        this->openFile(m_settings->getBookmarks().at(bookmarkIndex).getPath());
    }
}

void MainWindow::deleteBookmark()
{
    if (m_act_bookmark_delete->data().isNull())
        return;

    m_settings->deleteBookmark(m_act_bookmark_delete->data().toInt());
    populateBookmarks();
    m_act_bookmark_delete->setData(QVariant());
}

void MainWindow::on_filesView_currentChanged(const FileInfo &info)
{
#ifdef DEBUG_MAIN_WINDOW
    qDebug() << "MainWindow::on_filesView_currentChanged" << info.getDebugInfo();
#endif
    this->setCursor(Qt::BusyCursor);

    m_lineEdit_path->setText(info.getPath());
    m_picture_item->setPixmap(info);
}

void MainWindow::openFile(const FileInfo &info)
{
#ifdef DEBUG_MAIN_WINDOW
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::openFile" << info.getDebugInfo();
#endif

    if (!info.isValid())
    {
        return;
    }

    m_flag_opening = true;
    m_view_filesystem->setCurrentIndex(m_model_filesystem->index(info.getContainerPath()));
    m_view_files->setCurrentFile(info);
    m_picture_item->setFocus();
    m_flag_opening = false;
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        if (Helper::checkFileExtension(QFileInfo(event->mimeData()->urls().at(0).toLocalFile())))
        {
            event->acceptProposedAction();
        }
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
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

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::FocusOut)
    {
        if (obj == m_lineEdit_path)
        {
            on_lineEditPath_focus_lost();
        }
        else if (obj == m_comboBox_zoom)
        {
            on_comboBoxZoom_focus_lost();
        }
    }

    // standard event processing
    return QObject::eventFilter(obj, event);
}

void MainWindow::on_lineEditPath_editingFinished()
{
#ifdef DEBUG_MAIN_WINDOW
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::on_lineEditPath_editingFinished" << this->m_lineEdit_path->text();
#endif
    FileInfo info = FileInfo(m_lineEdit_path->text());
    if (info.isValid())
    {
        this->openFile(m_lineEdit_path->text());
    }
    else
    {
        m_lineEdit_path->setText(m_view_files->getCurrentFileInfo().getPath());
        m_picture_item->setFocus();
    }
}

void MainWindow::on_lineEditPath_focus_lost()
{
#ifdef DEBUG_MAIN_WINDOW
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::on_lineEditPath_focus_lost" << this->m_lineEdit_path->text();
#endif
    m_lineEdit_path->setText(m_view_files->getCurrentFileInfo().getPath());
    m_lineEdit_path->clearFocus();
}

void MainWindow::refreshPath()
{
    this->openFile(m_view_files->getCurrentFileInfo());
}

void MainWindow::on_filesystemView_currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if (m_model_filesystem->isDir(current))
    {
        m_model_filesystem->fetchMore(current);
    }
    m_act_dirUp->setEnabled(current.parent().isValid());
    m_view_filesystem->scrollTo(current);

    this->setWindowTitle(m_model_filesystem->fileName(current) + " - " + QApplication::applicationName() + " " + QApplication::applicationVersion());


    if (m_flag_opening)
        return;

    QString currentContainer = m_model_filesystem->filePath(current);
    FileInfo info = FileInfo(currentContainer);
#ifdef DEBUG_MAIN_WINDOW
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::on_filesystemView_currentRowChanged" << info.getDebugInfo();
#endif

    m_view_files->setCurrentFile(info);
    m_lineEdit_path->setText(info.getPath());
    m_picture_item->setPixmap(info);
}

void MainWindow::on_filesView_item_activated(const QString &path)
{
#ifdef DEBUG_MAIN_WINDOW
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::on_filesView_item_activated" << path;
#endif

    m_view_filesystem->setCurrentIndex(m_model_filesystem->index(path));
    m_view_filesystem->expand(m_view_filesystem->currentIndex());
}

void MainWindow::toggleSidebar(bool value)
{
    m_splitter_sidebar->setVisible(value);
    if (m_act_fullscreen->isChecked())
    {
        m_menu_main->setVisible(value);
    }
}

void MainWindow::toggleFullscreen(bool value)
{
    m_act_sidebar->setChecked(!value);
    this->menuBar()->setVisible(!value);
    if (value)
    {

        showFullScreen();
    }
    else
    {
#ifdef linux
        /* Workaround for bug https://bugreports.qt.nokia.com//browse/QTBUG-10594 */
        showNormal();
#endif
        showMaximized();
    }
}

void MainWindow::dirUp()
{
    if (m_view_filesystem->currentIndex().parent().isValid())
    {
        m_view_filesystem->setCurrentIndex(m_view_filesystem->currentIndex().parent());
    }
}

void MainWindow::open()
{
    QString imageExtensions = "*." + Helper::getFiltersImage().join(" *.");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), m_model_filesystem->filePath(m_view_filesystem->currentIndex()),
                                                    tr("Zip files") + "(*.zip *.cbz);;" + tr("Images") + " (" + imageExtensions + ")");
    if (!fileName.isEmpty())
    {
        this->openFile(fileName);
    }
}

bool MainWindow::saveAs()
{
    FileInfo info = m_view_files->getCurrentFileInfo();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), info.getImageFileName());
    if (fileName.isEmpty())
    {
        return false;
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    if (info.isInArchive())
    {
        JlCompress::extractFile(info.getContainerPath(), info.zipImagePath(), fileName);
    }
    else
    {
        QFile::copy(info.getPath(), fileName);
    }
//    PictureLoader::getImage(info).save(fileName, "PNG");
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    return true;
}

void MainWindow::addBookmark()
{
    QInputDialog dialog(this);
    dialog.setWindowTitle(tr("Bookmark Page"));
    dialog.setLabelText(tr("Bookmark Name:"));
    dialog.setTextValue(m_model_filesystem->fileName(m_view_filesystem->currentIndex()) + " /" + m_view_files->getCurrentFileInfo().getImageFileName());
    if(dialog.exec() != QDialog::Accepted)
    {
        return;
    }
    m_settings->addBookmark(dialog.textValue(), m_view_files->getCurrentFileInfo().getPath());
    populateBookmarks();
}

void MainWindow::zoomReset()
{
    m_picture_item->setZoom(1);
}

void MainWindow::lockZoom()
{
    m_picture_item->setLockMode(LockMode::Zoom);
}

void MainWindow::lockNone()
{
    m_picture_item->setLockMode(LockMode::None);
}

void MainWindow::lockAutofit()
{
    m_picture_item->setLockMode(LockMode::Autofit);
}

void MainWindow::lockFitWidth()
{
    m_picture_item->setLockMode(LockMode::FitWidth);
}

void MainWindow::lockFitHeight()
{
    m_picture_item->setLockMode(LockMode::FitHeight);
}

void MainWindow::rotateLeft()
{
    m_picture_item->setRotation(m_picture_item->getRotation() - 10);
}

void MainWindow::rotateRight()
{
    m_picture_item->setRotation(m_picture_item->getRotation() + 10);
}

void MainWindow::rotateReset()
{
    m_picture_item->setRotation(0);
}

void MainWindow::settingsDialog()
{
    Settings_Dialog sd(m_settings, this);
    if (sd.exec() == QDialog::Accepted)
    {
        /* Update settings */
        if (m_settings->getHardwareAcceleration() != m_picture_item->getHardwareAcceleration())
        {
            m_picture_item->setHardwareAcceleration(m_settings->getHardwareAcceleration());
        }
        m_view_files->setThumbnailsSize(m_settings->getThumbnailSize());
        m_picture_item->setPixmap(m_view_files->getCurrentFileInfo());
    }
}

void MainWindow::about()
{
    QMessageBox *aboutBox = new QMessageBox(this);
    aboutBox->setWindowTitle("About " + QApplication::applicationName());
    aboutBox->setTextFormat(Qt::RichText);
    aboutBox->setText(
                QApplication::applicationName() + tr(" version ") + QApplication::applicationVersion() + "<br><br>" +
                tr("Author") + QString::fromUtf8(": Nikola Kocić") + "<br><br>" +
                tr("email") + ": <a href = \"mailto:nikolakocic@gmail.com\">nikolakocic@gmail.com</a><br><br>"+
                tr("Website") + ": <a href = \"http://nikola-kocic.github.com/KIV/\">http://nikola-kocic.github.com/KIV/</a>"
                );
    aboutBox->exec();
}

void MainWindow::website()
{
    QDesktopServices::openUrl(QUrl("http://nikola-kocic.github.com/KIV/"));
}

void MainWindow::toggleLargeIcons(bool value)
{
    int e;
    if (value)
    {

        //        e = QApplication::style()->pixelMetric(QStyle::PM_LargeIconSize);
        e = QApplication::style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    }
    else
    {
        e = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize);
    }

    QSize iconSize = QSize(e, e);
    m_toolbar->setIconSize(iconSize);
    //    this->toolbarDirectory->setIconSize(iconSize);


    m_settings->setLargeIcons(value);
}

void MainWindow::on_zoom_changed(qreal current, qreal previous)
{
    if (!m_picture_item->getDefaultZoomSizes().contains(current))
    {
        /* Add current Zoom value to comboBox */

        QString zoomText = QString::number((current * 100), 'f', 0) + "%";
        int insertIndex = 0;
        for (; insertIndex < m_comboBox_zoom->count(); ++insertIndex)
        {
            if (m_comboBox_zoom->itemData(insertIndex).toReal() > current)
            {
                break;
            }
        }

        m_comboBox_zoom->insertItem(insertIndex, zoomText, current);
        m_comboBox_zoom->setCurrentIndex(insertIndex);
    }
    else
    {
        /* Add select current Zoom value from comboBox */

        for (int existingIndex = 0; existingIndex < m_comboBox_zoom->count(); ++existingIndex)
        {
            if (Helper::FuzzyCompare(m_comboBox_zoom->itemData(existingIndex).toReal(), current))
            {
                m_comboBox_zoom->setCurrentIndex(existingIndex);
                break;
            }
        }
    }

    if (!m_picture_item->getDefaultZoomSizes().contains(previous))
    {
        /* Remove previous Zoom value if it's not in default zoom sizes */

        for (int existingIndex = 0; existingIndex < m_comboBox_zoom->count(); ++existingIndex)
        {
            if (Helper::FuzzyCompare(m_comboBox_zoom->itemData(existingIndex).toReal(), previous))
            {
                m_comboBox_zoom->removeItem(existingIndex);
                break;
            }
        }
    }
}

void MainWindow::on_comboBoxZoom_TextChanged()
{
    QString zoomvalue = m_comboBox_zoom->lineEdit()->text();
    zoomvalue = zoomvalue.remove('%');

    bool ok;
    int dec = zoomvalue.toInt(&ok, 10);

    if (ok)
    {
        qreal z = (qreal)dec / 100;
        m_picture_item->setZoom(z);
    }
    else
    {
        on_zoom_changed(m_picture_item->getZoom(), m_picture_item->getZoom());
    }
}

void MainWindow::on_comboBoxZoom_focus_lost()
{
#ifdef DEBUG_MAIN_WINDOW
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::on_comboBoxZoom_focus_lost" << m_comboBox_zoom->lineEdit()->text();
#endif
    on_zoom_changed(m_picture_item->getZoom(), m_picture_item->getZoom());
    m_comboBox_zoom->clearFocus();
}

void MainWindow::on_comboBoxZoom_activated(const int &index)
{
    m_picture_item->setZoom(m_comboBox_zoom->itemData(index).toReal());
}

void MainWindow::toggleShowThumbnails(bool b)
{
    m_view_files->setShowThumbnails(b);
}

void MainWindow::updateActions()
{
    this->unsetCursor();

    bool enableActions = !m_picture_item->isPixmapNull();

    m_act_save->setEnabled(enableActions);
    m_act_zoomIn->setEnabled(enableActions);
    m_act_zoomOut->setEnabled(enableActions);
    m_act_rotateLeft->setEnabled(enableActions);
    m_act_rotateRight->setEnabled(enableActions);
    m_act_zoomReset->setEnabled(enableActions);
    m_act_fitToWindow->setEnabled(enableActions);
    m_act_fitToWidth->setEnabled(enableActions);
    m_act_fitToHeight->setEnabled(enableActions);
    m_act_rotateReset->setEnabled(enableActions);
    m_comboBox_zoom->setEnabled(enableActions);
}

void MainWindow::on_bookmark_customContextMenuRequested(const QPoint &pos)
{
    if (QAction *action = m_menu_bookmarks->actionAt(pos))
    {
        if (action->data().isNull())
            return;

        m_act_bookmark_delete->setData(action->data());
        m_menu_context_bookmark->popup(m_menu_bookmarks->mapToGlobal(pos));
    }
}

void MainWindow::on_view_mode_list_triggered()
{
    m_view_files->setViewMode(FileViewMode::List);
}

void MainWindow::on_view_mode_details_triggered()
{
    m_view_files->setViewMode(FileViewMode::Details);
}

void MainWindow::on_view_mode_icons_triggered()
{
    m_view_files->setViewMode(FileViewMode::Icons);
}
