#include <QtGui/qapplication.h>
#include "KomicViewer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("KomicViewer");
    app.setOrganizationName("KomicViewer");
    app.setApplicationVersion("0.7");

    QStringList args = QApplication::arguments();
    KomicViewer komicViewer(args);
    komicViewer.showMaximized();
    return app.exec();
}
