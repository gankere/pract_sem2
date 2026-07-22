#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTcpSocket>
#include <QJsonObject>

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);

    QString getClientName() const { return clientName; }
    QString getRoomCode() const { return roomCode; }
    QString getPodcastName() const { return podcastName; }
    bool isCreatorMode() const { return creatorMode; }

signals:
    void createRoomRequested(const QString &podcastName);
    void joinRoomRequested(const QString &roomCode, const QString &clientName);

private slots:
    void onCreateRoomClicked();
    void onJoinRoomClicked();
    
    // обработка ответов сервера
    void onServerResponse();
    void onConnected();

private:
    // отправка данных на сервер
    void sendToServer(const QJsonObject &message);
    
    QLineEdit *nameEdit;
    QLineEdit *codeEdit;
    QPushButton *createButton;
    QPushButton *joinButton;
    QLabel *errorLabel;

    QTcpSocket *socket;

    QString clientName;
    QString roomCode;
    QString podcastName;
    bool creatorMode;
};

#endif // STARTDIALOG_H