#include "main_window.h"

#include <QtGui/qapplication.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("KomicViewer");
    app.setOrganizationName("KomicViewer");
    app.setApplicationVersion("0.9");

    MainWindow komicViewer;
    komicViewer.showMaximized();
    return app.exec();
}
