#include "widgets/main_window.h"

#include <qglobal.h>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QBoxLayout>
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QMimeData>
#include <QUrl>

//#define DEBUG_MAIN_WINDOW
#ifdef DEBUG_MAIN_WINDOW
#include "helper.h"
#endif

#include "widgets/settings_dialog.h"
#include "settings.h"

MainWindow::MainWindow(const DataLoader& data_loader,
                       const IPictureLoader *const picture_loader,
                       const IArchiveExtractor* const archive_extractor,
                       QWidget *parent,
                       Qt::WindowFlags f)
    : QMainWindow(parent, f)
    , m_data_loader(data_loader)
    , m_model_filesystem(new CustomFileSystemModel(this))

    , m_settings(new Settings())

    , m_view_files(
          new ViewFilesUnified(
              archive_extractor,
              m_model_filesystem,
              this))

    , m_splitter_main(new QSplitter(Qt::Horizontal, this))
    , m_picture_item(new PictureItem(data_loader, picture_loader, m_settings->getData(), this))

    , m_menu_main(new QMenuBar(this))
    , m_toolbar(new QToolBar(this))
    , m_comboBox_zoom(new ZoomWidget(
                          QVector<qreal>()
                          << 0.1 << 0.25 << 0.5 << 0.75 << 1.0 << 1.25 << 1.5
                          << 2.0 << 3.0 << 4.0 << 5.0 << 6.0 << 7.0 << 8.0
                          << 9.0 << 10.0,
                          this))

    , m_menu_bookmarks(new QMenu(tr("&Bookmarks"), this))
    , m_menu_history(new QMenu(tr("&History"), this))
    , m_menu_context_picture(new QMenu(this))
    , m_menu_context_bookmark(new QMenu(this))
    , m_act_bookmark_delete(new QAction(tr("&Delete Bookmark"), this))
    , m_act_focus_location(new QAction(tr("Fo&cus location bar"), this))

    , m_act_open(new QAction(tr("&Open..."), this))
    , m_act_save(new QAction(tr("&Save Image As..."), this))
    , m_act_imageNext(new QAction(tr("&Next"), this))
    , m_act_imagePrevious(new QAction(tr("&Previous"), this))
    , m_act_dirUp(new QAction(tr("Go &Up"), this))
    , m_act_back(new QAction(tr("&Back"), this))
    , m_act_forward(new QAction(tr("&Forward"), this))
    , m_act_refreshPath(new QAction(tr("&Refresh"), this))
    , m_act_showInFileBrowser(new QAction(tr("Show in File Browser"), this))
    , m_act_exit(new QAction(tr("E&xit"), this))
    , m_act_bookmark_add(new QAction(tr("Bookmark &This Image"), this))
    , m_act_bookmark_active_item(nullptr)

    , m_act_zoomIn(new QAction(tr("Zoom &In"), this))
    , m_act_zoomOut(new QAction(tr("Zoom &Out"), this))
    , m_act_zoomReset(new QAction(tr("&Original Size (100%)"), this))
    , m_act_rotateLeft(new QAction(tr("Rotate &Left"), this))
    , m_act_rotateRight(new QAction(tr("Rotate &Right"), this))
    , m_act_rotateReset(new QAction(tr("R&eset Rotation"), this))
    , m_act_fitToWindow(new QAction(tr("&Fit to Window"), this))
    , m_act_fitToWidth(new QAction(tr("Fit &Width"), this))
    , m_act_fitToHeight(new QAction(tr("Fit &Height"), this))
    , m_act_lockNone(new QAction(tr("Lock &None"), this))
    , m_act_lockZoom(new QAction(tr("Lock Zoom &Value"), this))
    , m_act_lockAutofit(new QAction(tr("Lock Autofi&t"), this))
    , m_act_lockFitWidth(new QAction(tr("Lock Fit Wi&dth"), this))
    , m_act_lockFitHeight(new QAction(tr("Lock Fit Hei&ght"), this))

    , m_act_thumbnails(new QAction(tr("&Thumbnails"), this))
    , m_act_mode_list(new QAction(tr("&List"), this))
    , m_act_mode_details(new QAction(tr("&Details"), this))
    , m_act_mode_icons(new QAction(tr("&Icons"), this))
    , m_act_sidebar(new QAction(tr("Side&bar"), this))
    , m_act_fullscreen(new QAction(tr("&Full Screen"), this))

    , m_act_largeIcons(new QAction(tr("Large Toolbar &Icons"), this))
    , m_act_settings(new QAction(tr("&Settings..."), this))

    , m_act_webSite(new QAction(tr("&Web Site"), this))
    , m_act_about(new QAction(tr("&About"), this))

    , m_ignoreMouseButtonEvent(false)

{
    this->setAcceptDrops(true);
    QString startFilePath;
    if (QApplication::arguments().size() > 1)
    {
        startFilePath = QApplication::arguments().at(1);
    }
    else
    {
        startFilePath = m_settings->getLastPath();
    }
    m_urlNavigator = new UrlNavigator(m_model_filesystem,
                                      QUrl::fromLocalFile(startFilePath),
                                      this);

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
    vboxMain->setContentsMargins(0, 0, 0, 0);

    m_splitter_main->addWidget(m_view_files);
    m_splitter_main->setSizes(QList<int>() << 300);
    m_splitter_main->addWidget(m_picture_item);

    vboxMain->addWidget(m_toolbar);
    vboxMain->addWidget(m_splitter_main);

    this->setCentralWidget(content);

    /* End Layout */
    this->openFilePath(startFilePath);
}

MainWindow::~MainWindow()
{
    m_actions_icons.clear();
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
    DEBUGOUT << event->key();
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
    m_actions_icons[m_act_open] = QStringList() << "document-open";
    m_actions_icons[m_act_save] = QStringList() << "document-save-as";
    m_actions_icons[m_act_imageNext] = QStringList() << "media-skip-forward";
    m_actions_icons[m_act_imagePrevious] = QStringList() << "media-skip-backward";
    m_actions_icons[m_act_dirUp] = QStringList() << "go-up";
    m_actions_icons[m_act_back] = QStringList() << "go-previous";
    m_actions_icons[m_act_forward] = QStringList() << "go-next";
    m_actions_icons[m_act_refreshPath] = QStringList() << "view-refresh";
    m_actions_icons[m_act_exit] = QStringList() << "application-exit";
    m_actions_icons[m_act_zoomIn] = QStringList() << "zoom-in";
    m_actions_icons[m_act_zoomOut] = QStringList() << "zoom-out";
    m_actions_icons[m_act_zoomReset] = QStringList() << "zoom-original";
    m_actions_icons[m_act_rotateLeft] = QStringList() << "object-rotate-left";
    m_actions_icons[m_act_rotateRight] = QStringList() << "object-rotate-right";
    m_actions_icons[m_act_fitToWindow] = QStringList() << "zoom-fit-best";
    m_actions_icons[m_act_fitToWidth] = QStringList() << "zoom-fit-width";
    m_actions_icons[m_act_fitToHeight] = QStringList() << "zoom-fit-height";
    m_actions_icons[m_act_thumbnails] = QStringList() << "image-x-generic";
    m_actions_icons[m_act_sidebar] = QStringList() << "view-left-right";
    m_actions_icons[m_act_fullscreen] = QStringList() << "view-fullscreen";
    m_actions_icons[m_act_settings] = QStringList() << "configure"
                                                    << "gtk-preferences";
    updateIcons();

    m_act_save->setEnabled(false);
    m_act_dirUp->setEnabled(true);
    m_act_back->setEnabled(false);
    m_act_forward->setEnabled(false);
    m_act_zoomIn->setEnabled(false);
    m_act_zoomOut->setEnabled(false);
    m_act_zoomReset->setEnabled(false);
    m_act_rotateLeft->setEnabled(false);
    m_act_rotateRight->setEnabled(false);
    m_act_rotateReset->setEnabled(false);
    m_act_fitToWindow->setEnabled(false);
    m_act_fitToWidth->setEnabled(false);
    m_act_fitToHeight->setEnabled(false);

    m_act_open->setShortcut(QKeySequence::Open);
    m_act_save->setShortcut(QKeySequence::SaveAs);
    m_act_imageNext->setShortcut(QKeySequence::MoveToNextPage);
    m_act_imagePrevious->setShortcut(QKeySequence::MoveToPreviousPage);
    m_act_back->setShortcut(QKeySequence::Back);
    m_act_forward->setShortcut(QKeySequence::Forward);
    m_act_refreshPath->setShortcut(QKeySequence::Refresh);
    m_act_exit->setShortcut(QKeySequence::Quit);
    m_act_bookmark_add->setShortcut(Qt::CTRL | Qt::Key_D);
    m_act_zoomIn->setShortcut(QKeySequence::ZoomIn);
    m_act_zoomOut->setShortcut(QKeySequence::ZoomOut);
    m_act_zoomReset->setShortcut(Qt::CTRL | Qt::Key_0);
    m_act_fitToWindow->setShortcut(Qt::CTRL | Qt::Key_T);
    m_act_fullscreen->setShortcuts(QList<QKeySequence>()
                                   << Qt::Key_F11
                                   << QKeySequence(Qt::ALT | Qt::Key_Return));
    m_act_settings->setShortcut(QKeySequence::Preferences);
    m_act_about->setShortcut(QKeySequence::HelpContents);

    m_act_focus_location->setShortcuts(QList<QKeySequence>()
                                       << QKeySequence(Qt::ALT | Qt::Key_D)
                                       << QKeySequence(Qt::CTRL | Qt::Key_L));

    m_act_lockNone->setCheckable(true);
    m_act_lockZoom->setCheckable(true);
    m_act_lockAutofit->setCheckable(true);
    m_act_lockFitWidth->setCheckable(true);
    m_act_lockFitHeight->setCheckable(true);
    m_act_thumbnails->setCheckable(true);
    m_act_mode_list->setCheckable(true);
    m_act_mode_details->setCheckable(true);
    m_act_mode_icons->setCheckable(true);
    m_act_sidebar->setCheckable(true);
    m_act_fullscreen->setCheckable(true);
    m_act_largeIcons->setCheckable(true);

    m_act_mode_list->setChecked(true);
    m_act_sidebar->setChecked(true);
    m_act_lockNone->setChecked(true);

    /* Large icons are On by default but I want small icons by default */
    if (m_settings->getLargeIcons())
    {
        m_act_largeIcons->setChecked(true);
    }
    else
    {
        toggleLargeIcons(false);
    }

    m_act_exit->setMenuRole(QAction::QuitRole);
    m_act_settings->setMenuRole(QAction::PreferencesRole);
    m_act_about->setMenuRole(QAction::AboutRole);
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = m_menu_main->addMenu(tr("&File"));
    fileMenu->addAction(m_act_open);
    fileMenu->addAction(m_act_save);

    m_menu_bookmarks->addAction(m_act_bookmark_add);
    m_menu_bookmarks->addSeparator();

    m_menu_bookmarks->setContextMenuPolicy(Qt::CustomContextMenu);

    fileMenu->addMenu(m_menu_bookmarks);

    fileMenu->addSeparator();
    fileMenu->addAction(m_act_imageNext);
    this->addAction(m_act_imageNext);
    fileMenu->addAction(m_act_imagePrevious);
    this->addAction(m_act_imagePrevious);
    fileMenu->addMenu(m_menu_history);

    fileMenu->addSeparator();
    fileMenu->addAction(m_act_dirUp);
    fileMenu->addAction(m_act_back);
    fileMenu->addAction(m_act_forward);
    fileMenu->addAction(m_act_refreshPath);
    fileMenu->addAction(m_act_showInFileBrowser);

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
    m_toolbar->layout()->setContentsMargins(0, 0, 0, 0);

    m_toolbar->addAction(m_act_sidebar);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_act_back);
    m_toolbar->addAction(m_act_forward);
    m_toolbar->addAction(m_act_dirUp);
    m_toolbar->addAction(m_act_refreshPath);
    m_toolbar->addWidget(m_urlNavigator);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_act_imagePrevious);
    m_toolbar->addAction(m_act_imageNext);
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
    m_menu_context_picture->addAction(m_act_imageNext);
    m_menu_context_picture->addAction(m_act_imagePrevious);
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

    m_menu_context_bookmark->addAction(m_act_bookmark_delete);
    this->addAction(m_act_focus_location);
}

void MainWindow::connectActions()
{
    connect(m_act_open, &QAction::triggered, this, &MainWindow::openFileDialog);
    connect(m_act_save, &QAction::triggered, this, &MainWindow::saveAs);
    connect(m_act_exit, &QAction::triggered, this, &MainWindow::close);
    connect(m_act_back, &QAction::triggered, m_urlNavigator, &UrlNavigator::goBack);
    connect(m_act_forward, &QAction::triggered, m_urlNavigator, &UrlNavigator::goForward);

    connect(m_act_bookmark_add, &QAction::triggered, this, &MainWindow::addBookmark);

    connect(m_act_settings, &QAction::triggered, this, &MainWindow::settingsDialog);
    connect(m_act_imagePrevious, &QAction::triggered,
            m_view_files, &ViewFilesUnified::imagePrevious);
    connect(m_act_imageNext, &QAction::triggered,
            m_view_files, &ViewFilesUnified::imageNext);


    connect(m_act_thumbnails, &QAction::toggled,
            this, &MainWindow::toggleShowThumbnails);
    connect(m_act_fullscreen, &QAction::toggled,
            this, &MainWindow::toggleFullscreen);
    connect(m_act_largeIcons, &QAction::toggled,
            this, &MainWindow::toggleLargeIcons);
    connect(m_act_sidebar, &QAction::toggled,
            this, &MainWindow::toggleSidebar);
    connect(m_act_mode_list, &QAction::triggered,
            this, &MainWindow::on_view_mode_list_triggered);
    connect(m_act_mode_details, &QAction::triggered,
            this, &MainWindow::on_view_mode_details_triggered);
    connect(m_act_mode_icons, &QAction::triggered,
            this, &MainWindow::on_view_mode_icons_triggered);

    connect(m_act_rotateLeft, &QAction::triggered, this, &MainWindow::rotateLeft);
    connect(m_act_rotateRight, &QAction::triggered, this, &MainWindow::rotateRight);
    connect(m_act_rotateReset, &QAction::triggered, this, &MainWindow::rotateReset);
    connect(m_act_zoomIn, &QAction::triggered, m_comboBox_zoom, &ZoomWidget::zoomIn);
    connect(m_act_zoomOut, &QAction::triggered,
            m_comboBox_zoom, &ZoomWidget::zoomOut);
    connect(m_act_zoomReset, &QAction::triggered, this, &MainWindow::zoomReset);
    connect(m_act_fitToWindow, &QAction::triggered,
            m_picture_item, &PictureItem::fitToScreen);
    connect(m_act_fitToWidth, &QAction::triggered,
            m_picture_item, &PictureItem::fitWidth);
    connect(m_act_fitToHeight, &QAction::triggered,
            m_picture_item, &PictureItem::fitHeight);

    connect(m_act_lockNone, &QAction::triggered, this, &MainWindow::lockNone);
    connect(m_act_lockZoom, &QAction::triggered, this, &MainWindow::lockZoom);
    connect(m_act_lockAutofit, &QAction::triggered, this, &MainWindow::lockAutofit);
    connect(m_act_lockFitHeight, &QAction::triggered,
            this, &MainWindow::lockFitHeight);
    connect(m_act_lockFitWidth, &QAction::triggered,
            this, &MainWindow::lockFitWidth);

    connect(m_act_about, &QAction::triggered, this, &MainWindow::about);
    connect(m_act_webSite, &QAction::triggered, this, &MainWindow::website);

    connect(m_urlNavigator, &UrlNavigator::urlChanged,
            m_view_files, &ViewFilesUnified::setLocationUrl);
    connect(m_urlNavigator, &UrlNavigator::urlChanged,
            this, &MainWindow::setLocationUrl);
    connect(m_view_files, &ViewFilesUnified::urlChanged,
            m_urlNavigator, &UrlNavigator::setLocationUrl);
    connect(m_view_files, &ViewFilesUnified::urlChanged,
            this, &MainWindow::setLocationUrl);

    connect(m_picture_item, &PictureItem::imageChanged,
            this, &MainWindow::updateActions);
    connect(m_picture_item, &PictureItem::zoomChanged,
            m_comboBox_zoom, &ZoomWidget::on_zoomChanged);
    connect(m_picture_item, &PictureItem::customContextMenuRequested,
            this, &MainWindow::on_customContextMenuRequested);

    connect(m_comboBox_zoom, &ZoomWidget::zoomChanged,
            m_picture_item, &PictureItem::setZoom);

    connect(m_act_refreshPath, &QAction::triggered, this, &MainWindow::refreshPath);
    connect(m_act_showInFileBrowser, &QAction::triggered,
            this, &MainWindow::showInFileBrowser);
    connect(m_act_dirUp, &QAction::triggered, m_view_files, &ViewFilesUnified::dirUp);

    connect(m_act_bookmark_delete, &QAction::triggered,
            this, &MainWindow::deleteBookmark);
    connect(m_act_focus_location, &QAction::triggered,
            m_urlNavigator, &UrlNavigator::focus);

    connect(m_menu_history, &QMenu::aboutToShow,
            this, &MainWindow::populateHistoryMenu);
    connect(m_menu_history, &QMenu::triggered,
            this, &MainWindow::on_historyMenuTriggered);

    connect(m_menu_bookmarks, &QMenu::aboutToShow,
            this, &MainWindow::populateBookmarks);
    connect(m_menu_bookmarks, &QMenu::aboutToHide,
            this, &MainWindow::on_bookmark_menu_aboutToHide);
    connect(m_menu_bookmarks, &QMenu::customContextMenuRequested,
            this, &MainWindow::on_bookmark_customContextMenuRequested);
    connect(m_menu_bookmarks, &QMenu::triggered,
            this, &MainWindow::on_bookmark_triggered);

    connect(m_urlNavigator, &UrlNavigator::historyChanged,
            this, &MainWindow::on_urlHistoryChanged);


    foreach (QAction *action, QList<QAction*>() << m_act_back << m_act_forward)
    {
        QWidget *action_widget = m_toolbar->widgetForAction(action);
        action_widget->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(action_widget, &QWidget::customContextMenuRequested,
                this, &MainWindow::on_historyMenuRequested);
    }
}

void MainWindow::updateIcons()
{
    for (QHash<QAction*, QStringList>::iterator it = m_actions_icons.begin();
         it != m_actions_icons.end();
         ++it)
    {
        QAction *action = it.key();
        QStringList icons = it.value();
        QIcon icon;

        for (QStringList::const_iterator icon_it = icons.constBegin();
             icon_it != icons.constEnd();
             ++icon_it)
        {
            icon = QIcon::fromTheme((*icon_it));
            if (! icon.isNull())
            {
                action->setIcon(icon);
                break;
            }
        }
        if (action->icon().isNull())
        {
#ifndef ANDROID
            const QString icon_path_template = QString("%1/icons/default/%3/%2.png")
                    .arg(QCoreApplication::applicationDirPath())
                    .arg(icons[0]);
#else
            const QString icon_path_template = QString(":/res/icons/default/%2/%1.png")
                    .arg(icons[0]);
#endif
            const QString icon_path_22 = icon_path_template.arg("22x22");
            const QString icon_path_16 = icon_path_template.arg("16x16");

            if (QFile::exists(icon_path_22))
            {
                icon.addFile(icon_path_22, QSize(22, 22));
            }
            if (QFile::exists(icon_path_16))
            {
                icon.addFile(icon_path_16, QSize(16, 16));
            }
            action->setIcon(icon);
        }
    }
}

void MainWindow::populateBookmarks()
{
    /* Cache: If bookmarks menu is not empty that means
     * history did not change. */
    if (!m_menu_bookmarks->actions().last()->data().isNull())
    {
        return;
    }

    for (const Bookmark *bookmark : m_settings->getBookmarks())
    {
        QAction *act_bookmark = new QAction(bookmark->getName(),
                                            m_menu_bookmarks);
        act_bookmark->setData(bookmark->getPath());
        m_menu_bookmarks->addAction(act_bookmark);
#ifdef DEBUG_MAIN_WINDOW
        DEBUGOUT << "added bookmark" << bookmark->text()
                 << bookmark->data().toString();
#endif
    }
}

void MainWindow::on_customContextMenuRequested(const QPoint &pos)
{
    m_menu_context_picture->popup(m_picture_item->mapToGlobal(pos));
}

void MainWindow::on_bookmark_triggered(QAction *action)
{
    if (action->data().isNull())
    { return; }
#ifdef DEBUG_MAIN_WINDOW
    DEBUGOUT;
#endif
    this->openFilePath(action->data().toString());
}

void MainWindow::deleteBookmark()
{
    if (m_act_bookmark_active_item == nullptr)
    { return; }

    m_settings->deleteBookmark(m_act_bookmark_active_item->data().toString());
    m_menu_bookmarks->removeAction(m_act_bookmark_active_item);
    delete m_act_bookmark_active_item;
    m_act_bookmark_active_item = nullptr;
}

bool MainWindow::spreadUrl(const QUrl &url)
{
    if (m_view_files->setLocationUrl(url)) {
        m_urlNavigator->setLocationUrl(url);
        return true;
    } else {
        QMessageBox::critical(
                    this, QApplication::applicationName(),
                    tr("File not found: ").append(url.toLocalFile()),
                    QMessageBox::Close);
        return false;
    }
}

bool MainWindow::setLocationUrl(const QUrl &url)
{
    const QString filePath = url.toLocalFile();
    const FileInfo fileinfo(filePath);

    m_picture_item->setFocus();

    const QFileInfo fi(filePath);

    // TODO
    //m_act_dirUp->setEnabled(!fileinfo.isContainerRoot());
    this->setWindowTitle(
                fi.fileName() + " - " +
                QApplication::applicationName() + " " +
                QApplication::applicationVersion());

    m_picture_item->setPixmap(fileinfo);

    return true;
}

bool MainWindow::openUrl(const QUrl &url)
{
    if (spreadUrl(url)) {
        return setLocationUrl(url);
    }
    return false;
}

bool MainWindow::openFilePath(const QString &path) {
    return openUrl(QUrl::fromLocalFile(path));
}

bool MainWindow::openFile(const FileInfo &info)
{ return openFilePath(info.getPath()); }

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        const QString filePath = event->mimeData()->urls().first().toLocalFile();
        const bool supported = Helper::isSupportedFileType(filePath, m_data_loader);
        if (supported)
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
        this->openUrl(event->mimeData()->urls().first());
    }
    event->acceptProposedAction();
    return QMainWindow::dropEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    if (Helper::getFiltersImage().contains("svg"))
    {
        m_windowIcon = QIcon(QApplication::applicationDirPath()
                             + "/icons/kiv.svg");
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
        m_toolbar->setVisible(value);
    }
}

void MainWindow::toggleFullscreen(bool value)
{
    m_act_sidebar->setChecked(!value);
    this->menuBar()->setVisible(!value);
    m_toolbar->setVisible(!value);
    if (value)
    {

        showFullScreen();
    }
    else
    {
#ifdef linux
        /* Workaround for bug
         * https://bugreports.qt-project.org/browse/QTBUG-10594 */
        showNormal();
#endif
        showMaximized();
    }
}

void MainWindow::openFileDialog()
{
    const QString imageExtensions =
            "*." + Helper::getFiltersImage().join(" *.");
    const QString fileSystemPath = m_view_files->getCurrentFileInfo().getIdentifiers().parentIdentifier;
    const QFileInfo fi(fileSystemPath);
    const QString folderPath = fi.isDir() ? fi.canonicalFilePath() : fi.canonicalPath();

    const QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                folderPath,
                tr("Zip files") + "(*.zip *.cbz);;" + tr("Images")
                + " (" + imageExtensions + ")");
    if (!fileName.isEmpty())
    {
        this->openFilePath(fileName);
    }
}

void MainWindow::saveAs()
{
    const QString filePath = m_view_files->getCurrentFileInfo().getPath();
    const QFileInfo fi(filePath);
    const QString fileName = QFileDialog::getSaveFileName(
                this, tr("Save File") , fi.fileName());
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

void MainWindow::showInFileBrowser()
{
    const QString fileSystemPath = m_view_files->getCurrentFileInfo().getIdentifiers().parentIdentifier;
    const QFileInfo fi(fileSystemPath);
    Helper::showInFileBrowser(fi);
}

void MainWindow::addBookmark()
{
    QInputDialog dialog(this);
    dialog.setWindowTitle(tr("Bookmark Image"));
    dialog.setLabelText(tr("Bookmark Name:"));
    const QString path = m_view_files->getCurrentFileInfo().getPath();
    dialog.setTextValue(path);

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }
    const QString bookmarkName = dialog.textValue();
    const QString bookmarkPath = m_view_files->getCurrentFileInfo().getPath();
    bool result = m_settings->addBookmark(bookmarkName, bookmarkPath);
    if (result == false)
    {
        QMessageBox::information(this, QApplication::applicationName(),
                                 tr("Bookmark already exists"));
    }
    else
    {
        QAction *bookmark = new QAction(bookmarkName, m_menu_bookmarks);
        bookmark->setData(bookmarkPath);
        m_menu_bookmarks->addAction(bookmark);
    }
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
        m_picture_item->settingsUpdated(m_settings->getData(), m_view_files->getCurrentFileInfo());
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
                QApplication::applicationName() + " " + tr("version") + " "
                + QApplication::applicationVersion() + "<br><br>"
                + tr("Author: ")
                + QString::fromUtf8("Nikola Kocić")
                + "<br><br>" + tr("Website: ")
                + "<a href = \"https://github.com/nikola-kocic/KIV\">"
                "https://github.com/nikola-kocic/KIV</a>"
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
    if (QAction *action = m_menu_bookmarks->actionAt(pos))
    {
        if (action->data().isNull())
        { return; }

        m_act_bookmark_active_item = action;
        m_menu_context_bookmark->popup(m_menu_bookmarks->mapToGlobal(pos));
    }
}

void MainWindow::on_bookmark_menu_aboutToHide()
{
    m_act_bookmark_active_item = nullptr;
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

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (m_ignoreMouseButtonEvent) {
        m_ignoreMouseButtonEvent = false;
        return;
    }

    if (!isPosInPictureItem(event->globalPos()))
    {
        return;
    }

    const Qt::MouseButtons buttons = event->buttons();
    if (buttons != Qt::NoButton) {
        return;
    }

    const Qt::MouseButton button = event->button();

    switch (button) {
    case Qt::MiddleButton: {
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

        case MiddleClickAction::NextImage:
            m_view_files->imageNext();
            break;

        case MiddleClickAction::Quit:
            close();
            break;

        case MiddleClickAction::Boss:
            showMinimized();
            break;

        default:
            break;
        }
        break;
    }
    case Qt::LeftButton: {
        switch (m_settings->getLeftClick())
        {
        case LeftClickAction::ChangeImage: {
            const int widgetWidth = m_picture_item->width();
            const QPointF localPos = m_picture_item->mapFromGlobal(event->globalPos());
            if (localPos.x() < widgetWidth / 2.0) {
                m_view_files->imagePrevious();
            } else {
                m_view_files->imageNext();
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case Qt::BackButton: {
        m_view_files->imageNext();
        break;
    }
    case Qt::ForwardButton: {
        m_view_files->imagePrevious();
        break;
    }
    default:
        break;
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton
            && isPosInPictureItem(event->globalPos())
            && m_settings->getLeftClick() == LeftClickAction::BeginDrag)
    {
        m_act_fullscreen->toggle();
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (!isPosInPictureItem(event->globalPosition().toPoint()))
    {
        return;
    }

    /* event->delta() > 0 == Up
       event->delta() < 0 == Down */
    if ((Qt::ControlModifier == event->modifiers())
        ||((Qt::NoModifier == event->modifiers())
           && (WheelAction::Zoom == m_settings->getWheel())))
    {
        if (event->angleDelta().y() < 0)
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
        /* If image can't be scrolled, change image if necessary */
        if (WheelAction::ChangeImage == m_settings->getWheel())
        {
            if (event->angleDelta().y() < 0)
            {
                m_view_files->imageNext();
            }
            else
            {
                m_view_files->imagePrevious();
            }
        }
        /* Scroll image */
        else if (WheelAction::Scroll == m_settings->getWheel())
        {
            m_picture_item->scrollImageVertical(event->angleDelta().y());
        }
    }
    else if ((Qt::ControlModifier | Qt::ShiftModifier) == event->modifiers())
    {
        /* For standard scroll (+-120), zoom +-25% */
        m_comboBox_zoom->setZoom(m_comboBox_zoom->getZoom()
                                 * (1 + ((event->angleDelta().y() / 4.8) / 100)));
    }
    else if (Qt::ShiftModifier == event->modifiers())
    {
        m_picture_item->scrollImageVertical(event->angleDelta().y());
    }
    else if (Qt::AltModifier == event->modifiers())
    {
        m_picture_item->scrollImageHorizontal(event->angleDelta().y());
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::ActivationChange)
    {
        if (this->isActiveWindow())
        {
            if (QApplication::mouseButtons() & Qt::LeftButton) {
                m_ignoreMouseButtonEvent = true;
            }
        }
    }
}

/* History start */

void MainWindow::populateHistoryMenu()
{
    /* Cache: If history menu is not empty that means history did not change. */
    if (!m_menu_history->isEmpty())
    {
        return;
    }
    const QList<QUrl> history = m_urlNavigator->getHistory();
    for (int i = 0; i < history.size(); ++i)
    {
        const QString path = QDir::toNativeSeparators(
                    history.at(i).toLocalFile());
        QAction *const action = m_menu_history->addAction(path);
        action->setData(i);
        if (i == m_urlNavigator->historyIndex())
        {
            action->setCheckable(true);
            action->setChecked(true);
        }
    }
}

void MainWindow::on_historyMenuRequested(const QPoint & /*pos*/)
{
    m_menu_history->popup(QCursor::pos());
}

void MainWindow::on_historyMenuTriggered(QAction *action)
{
    m_urlNavigator->setHistoryIndex(action->data().toInt());
}

void MainWindow::on_urlHistoryChanged()
{
    /* Cache: Clear history menu entries when history changes
     * so it's repopulated on next request. */
    m_menu_history->clear();

    int historySize = m_urlNavigator->historySize();
    if (historySize <= 1)
    {
        return;
    }

    int historyIndex = m_urlNavigator->historyIndex();

    bool back_enabled = !(historyIndex == 0);
    m_act_back->setEnabled(back_enabled);

    bool forward_enabled = !(historyIndex == historySize - 1);
    m_act_forward->setEnabled(forward_enabled);
}

/* History end */
