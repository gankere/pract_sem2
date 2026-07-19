#include "mainwindow.h"
#include "startdialog.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    StartDialog *startDialog = new StartDialog;
    
    // Обработка создания комнаты
    QObject::connect(startDialog, &StartDialog::createRoomRequested,
        [startDialog](const QString &roomName) {
            qDebug() << "✅ Комната создана:" << roomName;

        });
    
    // Обработка присоединения к комнате
    QObject::connect(startDialog, &StartDialog::joinRoomRequested,
        [startDialog](const QString &roomCode, const QString &clientName) {
            qDebug() << " Подключение к комнате:" << roomCode << "как" << clientName;

        });
    
    startDialog->show();
    
    return app.exec();
}