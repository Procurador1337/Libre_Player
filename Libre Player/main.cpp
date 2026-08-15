#include "mainwindow.h"

#include <QApplication>
#include <QIcon>



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setWindowIcon(
        QIcon(":/icons/appicon.png")
        );

    MainWindow window;
    window.show();

    return app.exec();
}