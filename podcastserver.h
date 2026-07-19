#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>

struct RoomInfo {
    QString roomName;
    QString hostName;
    QList<struct ClientInfo> clients;
};

struct ClientInfo {
    QTcpSocket *socket;
    QString name;
    QString roomCode;
    bool isHost;  // true = ведущий, false = слушатель
};

class PodcastServer : public QObject
{
    Q_OBJECT

public:
    explicit PodcastServer(QObject *parent = nullptr);
    ~PodcastServer() override;
    
    bool startServer(quint16 port = 9999);
    void stopServer();

signals:
    void roomCreated(const QString &roomCode, const QString &roomName);
    void clientJoined(const QString &roomCode, const QString &clientName);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    void sendMessage(QTcpSocket *socket, const QJsonObject &message);
    void broadcastToRoom(const QString &roomCode, const QJsonObject &message, QTcpSocket *excludeSocket = nullptr);
    void removeClient(QTcpSocket *socket);
    int roomCounter;

    QTcpServer *tcpServer;
    QMap<QString, RoomInfo> rooms;
    QMap<QTcpSocket*, QString> socketToRoom;
    QMap<QTcpSocket*, QString> socketToName;
};