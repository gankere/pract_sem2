#include "mainwindow.h"
#include "startdialog.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    StartDialog startDialog;
    if (startDialog.exec() == QDialog::Accepted) {
        MainWindow window;

        if (!startDialog.getPodcastName().isEmpty()) {
            window.setPodcastName(startDialog.getPodcastName());
        }

        QString title = "Мини-подкаст";
        if (!startDialog.getPodcastName().isEmpty()) {
            title = "Мини-подкаст: " + startDialog.getPodcastName();
        }
        window.setWindowTitle(title);
        window.show();

        qDebug() << "Имя клиента:" << startDialog.getClientName();
        qDebug() << "Режим:" << (startDialog.isCreatorMode() ? "Создатель" : "Участник");
        if (!startDialog.isCreatorMode()) {
            qDebug() << "Код комнаты:" << startDialog.getRoomCode();
        }

        return app.exec();
    }

    return 0;
}