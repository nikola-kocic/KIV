#include "widgets/main_window.h"

#include <qglobal.h>
#include <QAction>
#include <QBoxLayout>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QMimeData>
#include <QUrl>

//#define DEBUG_MAIN_WINDOW
#ifdef DEBUG_MAIN_WINDOW
#include <QDebug>
#endif

#include "widgets/settings_dialog.h"
#include "settings.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent, f)
    , m_model_filesystem(new FileSystemModel(this))

    , m_settings(new Settings())

    , m_view_files(new ViewFiles(m_model_filesystem, this))

    , m_splitter_main(new QSplitter(Qt::Horizontal, this))
    , m_picture_item(new PictureItem(m_settings, this))

    , m_menu_main(new QMenuBar(this))
    , m_toolbar(new QToolBar(this))
    , m_location_widget(new LocationWidget(m_model_filesystem, this))
    , m_comboBox_zoom(new ZoomWidget(this))
{
    this->setAcceptDrops(true);

    updateSettings();
    createActions();
    createMenus();

    connectActions();


    /* Start Layout */

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    policy.setHorizontalStretch(1);
    policy.setVerticalStretch(0);
    m_picture_item->setSizePolicy(policy);

    this->setMenuBar(m_menu_main);

    QWidget *content = new QWidget(this);
    QVBoxLayout *vboxMain = new QVBoxLayout(content);
    vboxMain->setSpacing(0);
    vboxMain->setMargin(0);

    m_splitter_main->addWidget(m_view_files);
    m_splitter_main->setSizes(QList<int>() << 300);
    m_splitter_main->addWidget(m_picture_item);

    vboxMain->addWidget(m_toolbar);
    vboxMain->addWidget(m_splitter_main);

    this->setCentralWidget(content);

    /* End Layout */


    if (QApplication::arguments().size() > 1)
    {
        this->openFile(QApplication::arguments().at(1));
    }
    else
    {
        this->openFile(m_settings->getLastPath());
    }
}

MainWindow::~MainWindow()
{
    delete m_settings;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_settings->setLastPath(m_view_files->getCurrentFileInfo().getPath());
    return QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
#ifdef DEBUG_MAIN_WINDOW
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::keyPressEvent" << event->key();
#endif
    if (event->key() == Qt::Key_B)
    {
        showMinimized();
        event->accept();
    }
    else if (event->key() == Qt::Key_Escape)
    {
        m_act_fullscreen->setChecked(false);
    }
    else if (event->key() == Qt::Key_Minus)
    {
        m_act_zoomOut->trigger();
    }
    else if (event->key() == Qt::Key_Plus)
    {
        m_act_zoomIn->trigger();
    }
    else if (event->key() == Qt::Key_0)
    {
        m_act_zoomReset->trigger();
    }
    return QMainWindow::keyPressEvent(event);
}

void MainWindow::createActions()
{
    QDir appdir(QApplication::applicationDirPath());
    QIcon::setThemeSearchPaths(QStringList(QIcon::themeSearchPaths()) << appdir.path());

    static const char * GENERIC_ICON_TO_CHECK = "go-next";
    if (!QIcon::hasThemeIcon(GENERIC_ICON_TO_CHECK)) {
        /* If there is no default working icon theme then we should
           use an icon theme that we provide via a icons folder
           This case happens under Windows and Mac OS X
           This does not happen under GNOME or KDE */
        static const char * FALLBACK_ICON_THEME = "default";
        QIcon::setThemeName(FALLBACK_ICON_THEME);
    }


    /* File Actions */

    m_act_open = new QAction(QIcon::fromTheme("document-open"), tr("&Open..."), this);
    m_act_open->setShortcut(QKeySequence::Open);

    m_act_save = new QAction(QIcon::fromTheme("document-save-as"), tr("&Save Page As..."), this);
    m_act_save->setShortcut(QKeySequence::SaveAs);
    m_act_save->setEnabled(false);

    m_act_exit = new QAction(QIcon::fromTheme("application-exit"), tr("E&xit"), this);
    m_act_exit->setShortcut(QKeySequence::Quit);

    m_act_bookmark_add = new QAction(tr("Bookmark &This Page"), this);
    m_act_bookmark_add->setShortcut(tr("Ctrl+D"));

    m_act_pageNext = new QAction(QIcon::fromTheme("go-next"), tr("&Next"), this);
    m_act_pageNext->setShortcut(QKeySequence::MoveToNextPage);

    m_act_pagePrevious = new QAction(QIcon::fromTheme("go-previous"), tr("&Previous"), this);
    m_act_pagePrevious->setShortcut(QKeySequence::MoveToPreviousPage);


    /* View Actions */

    m_act_rotateLeft = new QAction(QIcon::fromTheme("object-rotate-left"), tr("Rotate &Left"), this);
    m_act_rotateLeft->setEnabled(false);

    m_act_rotateRight = new QAction(QIcon::fromTheme("object-rotate-right"), tr("Rotate &Right"), this);
    m_act_rotateRight->setEnabled(false);

    m_act_rotateReset = new QAction(tr("R&eset Rotation"), this);
    m_act_rotateReset->setEnabled(false);

    m_act_thumbnails = new QAction(QIcon::fromTheme("image-x-generic"), tr("&Thumbnails"), this);
    m_act_thumbnails->setCheckable(true);

    m_act_mode_icons = new QAction(tr("&Icons"), this);
    m_act_mode_icons->setCheckable(true);

    m_act_mode_details = new QAction(tr("&Details"), this);
    m_act_mode_details->setCheckable(true);

    m_act_mode_list = new QAction(tr("&List"), this);
    m_act_mode_list->setCheckable(true);
    m_act_mode_list->setChecked(true);


    m_act_fullscreen = new QAction(QIcon::fromTheme("view-fullscreen"), tr("&Full Screen"), this);
    m_act_fullscreen->setShortcuts(
                QList<QKeySequence>() << Qt::Key_F11 << QKeySequence(Qt::ALT | Qt::Key_Return));
    m_act_fullscreen->setCheckable(true);

    m_act_sidebar = new QAction(QIcon::fromTheme("view-split-left-right"),tr("Side&bar"), this);
    m_act_sidebar->setCheckable(true);
    m_act_sidebar->setChecked(true);


    /* Options Actions */

    m_act_largeIcons = new QAction(tr("Large Toolbar &Icons"), this);
    m_act_largeIcons->setCheckable(true);

    /* Large icons are On by default but I want small icons by default */
    if (m_settings->getLargeIcons())
    {
        m_act_largeIcons->setChecked(true);
    }
    else
    {
        toggleLargeIcons(false);
    }

    m_act_settings = new QAction(QIcon::fromTheme("configure", QIcon::fromTheme("gtk-preferences")), tr("&Settings..."), this);
    m_act_settings->setMenuRole(QAction::PreferencesRole);
    m_act_settings->setShortcut(QKeySequence::Preferences);

    /* Zoom Actions */

    m_act_zoomIn = new QAction(QIcon::fromTheme("zoom-in"), tr("Zoom &In"), this);
    m_act_zoomIn->setShortcut(QKeySequence::ZoomIn);
    m_act_zoomIn->setEnabled(false);

    m_act_zoomOut = new QAction(QIcon::fromTheme("zoom-out"), tr("Zoom &Out"), this);
    m_act_zoomOut->setShortcut(QKeySequence::ZoomOut);
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
    m_act_about->setShortcut(QKeySequence::HelpContents);


    /* Toolbar Actions */

    m_act_refreshPath = new QAction(QIcon::fromTheme("view-refresh"), tr("&Refresh"), this);
    m_act_refreshPath->setShortcut(QKeySequence::Refresh);

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

    fileMenu->addSeparator();
    fileMenu->addAction(m_act_pageNext);
    this->addAction(m_act_pageNext);
    fileMenu->addAction(m_act_pagePrevious);
    this->addAction(m_act_pagePrevious);

    fileMenu->addSeparator();

    fileMenu->addAction(m_act_exit);


    QMenu *imageMenu = m_menu_main->addMenu(tr("&Image"));
    imageMenu->addSeparator();
    imageMenu->addAction(m_act_zoomIn);
    this->addAction(m_act_zoomIn);
    imageMenu->addAction(m_act_zoomOut);
    this->addAction(m_act_zoomOut);
    imageMenu->addAction(m_act_zoomReset);
    this->addAction(m_act_zoomReset);
    imageMenu->addSeparator();
    imageMenu->addAction(m_act_rotateLeft);
    imageMenu->addAction(m_act_rotateRight);
    imageMenu->addAction(m_act_rotateReset);
    imageMenu->addSeparator();
    imageMenu->addAction(m_act_fitToWindow);
    this->addAction(m_act_fitToWindow);
    imageMenu->addAction(m_act_fitToWidth);
    imageMenu->addAction(m_act_fitToHeight);
    imageMenu->addSeparator();

    QActionGroup *lockActions = new QActionGroup(this);
    lockActions->addAction(m_act_lockZoom);
    lockActions->addAction(m_act_lockAutofit);
    lockActions->addAction(m_act_lockFitWidth);
    lockActions->addAction(m_act_lockFitHeight);
    lockActions->addAction(m_act_lockNone);

    imageMenu->addAction(m_act_lockNone);
    imageMenu->addAction(m_act_lockZoom);
    imageMenu->addAction(m_act_lockAutofit);
    imageMenu->addAction(m_act_lockFitWidth);
    imageMenu->addAction(m_act_lockFitHeight);


    QMenu *viewMenu = m_menu_main->addMenu(tr("&View"));
    viewMenu->addAction(m_act_thumbnails);

    viewMenu->addSeparator();
    QActionGroup *viewModes = new QActionGroup(this);
    viewModes->addAction(m_act_mode_list);
    viewModes->addAction(m_act_mode_details);
    viewModes->addAction(m_act_mode_icons);

    viewMenu->addAction(m_act_mode_list);
    viewMenu->addAction(m_act_mode_details);
    viewMenu->addAction(m_act_mode_icons);

    viewMenu->addSeparator();
    viewMenu->addAction(m_act_sidebar);
    this->addAction(m_act_fullscreen);
    viewMenu->addAction(m_act_fullscreen);


    QMenu *optionsMenu = m_menu_main->addMenu(tr("&Options"));

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
    m_toolbar->addWidget(m_location_widget);
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
    menuFit->addAction(m_act_fitToWindow);
    menuFit->addAction(m_act_fitToHeight);
    menuFit->addAction(m_act_fitToWidth);

    QMenu *menuLock = m_menu_context_picture->addMenu(tr("Lock"));
    menuLock->addAction(m_act_lockZoom);
    menuLock->addAction(m_act_lockAutofit);
    menuLock->addAction(m_act_lockFitWidth);
    menuLock->addAction(m_act_lockFitHeight);
    menuLock->addAction(m_act_lockNone);

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
    connect(m_act_zoomIn, SIGNAL(triggered()), m_comboBox_zoom, SLOT(zoomIn()));
    connect(m_act_zoomOut, SIGNAL(triggered()), m_comboBox_zoom, SLOT(zoomOut()));
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

    connect(m_location_widget, SIGNAL(locationChanged(FileInfo)), this, SLOT(openFile(FileInfo)));
    connect(m_view_files, SIGNAL(currentFileChanged(FileInfo)), this, SLOT(on_filesView_currentChanged(FileInfo)));

    connect(m_picture_item, SIGNAL(mousePress(QMouseEvent*const)),
            this, SLOT(on_pictureItemMousePress(QMouseEvent*const)));
    connect(m_picture_item, SIGNAL(mouseDoubleClick(QMouseEvent*const)),
            this, SLOT(on_pictureItemMouseDoubleClick(QMouseEvent*const)));
    connect(m_picture_item, SIGNAL(mouseWheel(QWheelEvent*const)),
             this, SLOT(on_pictureItemMouseWheel(QWheelEvent*const)));
    connect(m_picture_item, SIGNAL(imageChanged()), this, SLOT(updateActions()));
    connect(m_picture_item, SIGNAL(zoomChanged(qreal,qreal)), m_comboBox_zoom, SLOT(on_zoomChanged(qreal,qreal)));
    connect(m_picture_item, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_customContextMenuRequested(QPoint)));

    connect(m_comboBox_zoom, SIGNAL(zoomChanged(qreal)), m_picture_item, SLOT(setZoom(qreal)));

    connect(m_act_refreshPath, SIGNAL(triggered()), this, SLOT(refreshPath()));
    connect(m_act_dirUp, SIGNAL(triggered()), m_view_files, SLOT(dirUp()));

    connect(m_act_bookmark_delete, SIGNAL(triggered()), this, SLOT(deleteBookmark()));

}

void MainWindow::populateBookmarks()
{
    const QList<QAction*> oldActions = m_menu_bookmarks->actions();

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

    const QList<Bookmark *> bookmarks = m_settings->getBookmarks();
    for (int i = 0; i < bookmarks.size(); ++i)
    {
        QAction *bookmark = new QAction(bookmarks.at(i)->getName(), this);
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
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
    {
        if (action->data().isNull())
            return;
        int bookmarkIndex = action->data().toInt();
#ifdef DEBUG_MAIN_WINDOW
                qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::on_bookmark_triggered()";
#endif
        this->openFile(m_settings->getBookmarks().at(bookmarkIndex)->getPath());
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
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::on_filesView_currentChanged" << info.getDebugInfo();
#endif
    m_act_dirUp->setEnabled(!info.isContainerRoot());
    this->setWindowTitle(m_view_files->getCurrentFileInfo().getContainerName() + " - " + QApplication::applicationName() + " " + QApplication::applicationVersion());

    this->setCursor(Qt::BusyCursor);

    m_location_widget->setFileInfo(info);
    m_picture_item->setPixmap(info);
}

bool MainWindow::openFile(const FileInfo &info)
{
#ifdef DEBUG_MAIN_WINDOW
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "MainWindow::openFile" << info.getDebugInfo();
#endif

    if (!info.isValid())
    {
        return false;
    }

    m_view_files->setCurrentFile(info);
    m_picture_item->setFocus();
    return true;
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
    return QMainWindow::dragEnterEvent(event);
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
    return QMainWindow::dragMoveEvent(event);
}


void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
    return QMainWindow::dragLeaveEvent(event);
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->proposedAction() == Qt::CopyAction)
    {
        this->openFile(event->mimeData()->urls().at(0).toLocalFile());
    }
    event->acceptProposedAction();
    return QMainWindow::dropEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    if (Helper::getFiltersImage().contains("svg"))
    {
        m_windowIcon = QIcon(QApplication::applicationDirPath() + "/icons/kiv.svg");
        this->setWindowIcon(m_windowIcon);
    }
    return QMainWindow::showEvent(event);
}


void MainWindow::refreshPath()
{
    this->openFile(m_view_files->getCurrentFileInfo());
}


void MainWindow::toggleSidebar(bool value)
{
    m_view_files->setVisible(value);
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
        /* Workaround for bug https://bugreports.qt-project.org/browse/QTBUG-10594 */
        showNormal();
#endif
        showMaximized();
    }
}

void MainWindow::open()
{
    const QString imageExtensions = "*." + Helper::getFiltersImage().join(" *.");
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), m_view_files->getCurrentFileInfo().getContainerPath(),
                                                    tr("Zip files") + "(*.zip *.cbz);;" + tr("Images") + " (" + imageExtensions + ")");
    if (!fileName.isEmpty())
    {
        this->openFile(fileName);
    }
}

void MainWindow::saveAs()
{
    const FileInfo info = m_view_files->getCurrentFileInfo();
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), info.getImageFileName());
    if (fileName.isEmpty())
    {
        return;
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    m_view_files->saveCurrentFile(fileName);

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    return;
}

void MainWindow::addBookmark()
{
    QInputDialog dialog(this);
    dialog.setWindowTitle(tr("Bookmark Page"));
    dialog.setLabelText(tr("Bookmark Name:"));
    dialog.setTextValue(m_view_files->getCurrentFileInfo().getContainerName() + " /" + m_view_files->getCurrentFileInfo().getImageFileName());
    if(dialog.exec() != QDialog::Accepted)
    {
        return;
    }
    m_settings->addBookmark(dialog.textValue(), m_view_files->getCurrentFileInfo().getPath());
    populateBookmarks();
}

void MainWindow::zoomReset()
{
    m_comboBox_zoom->setZoom(1);
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
            m_picture_item->setPixmap(m_view_files->getCurrentFileInfo());
        }

        updateSettings();
    }
}

void MainWindow::updateSettings()
{
    m_view_files->setThumbnailsSize(m_settings->getThumbnailSize());
}

void MainWindow::about()
{
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle("About " + QApplication::applicationName());
    aboutBox.setTextFormat(Qt::RichText);
    aboutBox.setText(
                QApplication::applicationName() + " " + tr("version") + " " + QApplication::applicationVersion() + "<br><br>" +
                tr("Author") + QString::fromUtf8(": Nikola Kocić") + "<br><br>" +
                tr("Website") + ": <a href = \"https://github.com/nikola-kocic/KIV\">https://github.com/nikola-kocic/KIV</a>"
                );
    aboutBox.exec();
}

void MainWindow::website()
{
    QDesktopServices::openUrl(QUrl("https://github.com/nikola-kocic/KIV"));
}

void MainWindow::toggleLargeIcons(bool b)
{
    int size;
    if (b)
    {
        size = QApplication::style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    }
    else
    {
        size = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize);
    }

    m_toolbar->setIconSize(QSize(size, size));
    m_settings->setLargeIcons(b);
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
    if (const QAction *action = m_menu_bookmarks->actionAt(pos))
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

void MainWindow::on_pictureItemMousePress(const QMouseEvent * const event)
{
    if (event->buttons() != Qt::MiddleButton)
    {
        return;
    }

    switch (m_settings->getMiddleClick())
    {
    case MiddleClickAction::Fullscreen :
        m_act_fullscreen->toggle();
        break;

    case MiddleClickAction::AutoFit:
        m_picture_item->fitToScreen();
        break;

    case MiddleClickAction::ZoomReset:
        m_comboBox_zoom->setZoom(1);
        break;

    case MiddleClickAction::NextPage:
        m_view_files->pageNext();
        break;

    case MiddleClickAction::Quit:
        close();
        break;

    case MiddleClickAction::Boss:
        showMinimized();
        break;

    default: break;
    }
}

void MainWindow::on_pictureItemMouseDoubleClick(const QMouseEvent * const event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        m_act_fullscreen->toggle();
    }
}

void MainWindow::on_pictureItemMouseWheel(const QWheelEvent * const event)
{
    /* event->delta() > 0 == Up
       event->delta() < 0 == Down */
    if ((Qt::ControlModifier == event->modifiers()) ||
        ((Qt::NoModifier == event->modifiers()) && (WheelAction::Zoom == m_settings->getWheel())))
    {
        if (event->delta() < 0)
        {
            m_comboBox_zoom->zoomOut();
        }
        else
        {
            m_comboBox_zoom->zoomIn();
        }
    }
    else if (Qt::NoModifier == event->modifiers())
    {
        /* If page can't be scrolled, change page if necessary */
        if (WheelAction::ChangePage == m_settings->getWheel())
        {
            if (event->delta() < 0)
            {
                m_view_files->pageNext();
            }
            else
            {
                m_view_files->pagePrevious();
            }
        }
        /* Scroll page */
        else if (WheelAction::Scroll == m_settings->getWheel())
        {
            m_picture_item->scrollPageVertical(event->delta());
        }
    }
    else if ((Qt::ControlModifier | Qt::ShiftModifier) == event->modifiers())
    {
        /* For standard scroll (+-120), zoom +-25% */
        m_comboBox_zoom->setZoom(m_comboBox_zoom->getZoom() * (1 + ((event->delta() / 4.8) / 100)));
    }
    else if (Qt::ShiftModifier == event->modifiers())
    {
        m_picture_item->scrollPageVertical(event->delta());
    }
    else if (Qt::AltModifier == event->modifiers())
    {
        m_picture_item->scrollPageHorizontal(event->delta());
    }
}
