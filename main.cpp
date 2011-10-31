#include "main_window.h"

#include <QtGui/qapplication.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("KomicViewer");
    app.setOrganizationName("KomicViewer");
    app.setApplicationVersion("0.9");

    MainWindow komicViewer;

#ifdef WIN32
    QStringList l_libPaths;
    // Build plugins path
    QDir l_pluginsPath(qApp->applicationDirPath());
    l_pluginsPath.mkdir("plugins");
    l_pluginsPath.cd("plugins");

    // Set library paths
    l_libPaths << l_pluginsPath.path() << qApp->applicationDirPath();
    qApp->setLibraryPaths(l_libPaths);
#endif

    komicViewer.showMaximized();
    return app.exec();
}
