#include "KomicViewer.h"

#include <QtGui/qapplication.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("KomicViewer");
    app.setOrganizationName("KomicViewer");
    app.setApplicationVersion("0.8");

    QStringList args = QApplication::arguments();
    KomicViewer komicViewer(args);
    komicViewer.showMaximized();
    return app.exec();
}
