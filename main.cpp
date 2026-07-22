#include "mainwindow.h"
#include "startdialog.h"
#include "podcastserver.h" 
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Запуск сервера на порту 9999
    PodcastServer server;
    if (!server.startServer(9999)) {
        qDebug() << "❌ Критическая ошибка: не удалось запустить сервер!";
        return -1;
    }
    
    StartDialog *startDialog = new StartDialog;
    
    // Обработка создания комнаты
    QObject::connect(startDialog, &StartDialog::createRoomRequested,
        [&server](const QString &roomName) {
            qDebug() << "✅ Запрос на создание комнаты:" << roomName;
        });
    
    // Обработка присоединения к комнате
    QObject::connect(startDialog, &StartDialog::joinRoomRequested,
        [&server](const QString &roomCode, const QString &clientName) {
            qDebug() << "🔗 Запрос на подключение к комнате:" << roomCode << "как" << clientName;
        });
    
    startDialog->show();
    
    return app.exec();
}