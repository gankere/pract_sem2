#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Мини-подкаст");

    MainWindow w;
    w.setWindowTitle("Мини-подкаст с живым участием");
    w.resize(1200, 720);
    w.show();

    return app.exec();
}