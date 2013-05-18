#include <QApplication>

#include "widgets/main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("KIV");
    app.setOrganizationName("KIV");
    app.setApplicationVersion("0.3");

    MainWindow w;
    w.showMaximized();
    return app.exec();
}
