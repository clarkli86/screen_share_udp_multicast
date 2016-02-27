#include <QApplication>
#include <QEvent>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "serverframe.h"

int main(int argc, char *argv[]) {
    QApplication    app(argc, argv);

    ServerFrame	*server = new ServerFrame();

    app.setActiveWindow(server);
    server -> show();

    return app.exec();
}
