#include <QApplication>

#include "widgets/mainwindow.h"
#include "network/server.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    app.setOrganizationName("Groupe de projet sans nom");
    app.setApplicationName("Application sans nom");

    MainWindow mainWin;

    Server tcp_server;
    tcp_server.StartServer();

#if defined(Q_OS_SYMBIAN)
    mainWin.showMaximized();
#else
    mainWin.show();
#endif
    return app.exec();
}
