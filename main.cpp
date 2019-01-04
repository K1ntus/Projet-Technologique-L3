#include <QApplication>

#include "widgets/mainwindow.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    app.setOrganizationName("Groupe de projet sans nom");
    app.setApplicationName("Application sans nom");

    MainWindow mainWin;
#if defined(Q_OS_SYMBIAN)
    mainWin.showMaximized();
#else
    mainWin.show();
#endif
    return app.exec();
}
