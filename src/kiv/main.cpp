#include "main_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("KIV");
    app.setOrganizationName("KIV");
    app.setApplicationVersion("0.2");

    MainWindow w;
    w.showMaximized();
    return app.exec();
}
