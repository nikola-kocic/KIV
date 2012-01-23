#include "main_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("KIV");
    app.setOrganizationName("KIV");
    app.setApplicationVersion("0.2");

    MainWindow w;

#ifdef WIN32
    QStringList l_libPaths;
    /* Build plugins path */
    QDir l_pluginsPath(QApplication::applicationDirPath());
    l_pluginsPath.mkdir("plugins");
    l_pluginsPath.cd("plugins");

    /* Set library paths */
    l_libPaths << l_pluginsPath.path() << QApplication::applicationDirPath();
    QApplication::setLibraryPaths(l_libPaths);
#endif

    w.showMaximized();
    return app.exec();
}
