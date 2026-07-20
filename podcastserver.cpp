#include "podcastserver.h"
#include <QDebug>
#include <QRandomGenerator>

PodcastServer::PodcastServer(QObject *parent) 
    : QObject(parent), tcpServer(new QTcpServer(this)), roomCounter(0)
{
    connect(tcpServer, &QTcpServer::newConnection, this, &PodcastServer::onNewConnection);
}

PodcastServer::~PodcastServer()
{
    stopServer();
}

bool PodcastServer::startServer(quint16 port)
{
    if (tcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "✅ Сервер запущен на порту" << port;
        return true;
    }
    qDebug() << "❌ Ошибка запуска сервера:" << tcpServer->errorString();
    return false;
}

void PodcastServer::stopServer()
{
    tcpServer->close();
    qDebug() << "🛑 Сервер остановлен";
}

void PodcastServer::onNewConnection()
{
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    
    connect(clientSocket, &QTcpSocket::readyRead, this, &PodcastServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &PodcastServer::onDisconnected);
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    
    qDebug() << "🔌 Новое подключение";
}

void PodcastServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    // 1. Добавляем новые данные в буфер конкретного сокета
    socketBuffers[socket].append(socket->readAll());
    QByteArray &buffer = socketBuffers[socket];

    // 2. Обрабатываем буфер, пока в нем есть полные сообщения
    while (!buffer.isEmpty()) {
        
        // Обработка аудио-пакета
        if (buffer.startsWith("AUD:")) {
            const int HEADER_SIZE = 4;
            const int AUDIO_CHUNK_SIZE = 3200;
            const int TOTAL_AUDIO_SIZE = HEADER_SIZE + AUDIO_CHUNK_SIZE;

            // Если пакет пришел не целиком, ждем остальных данных (break выходит из while)
            if (buffer.size() < TOTAL_AUDIO_SIZE) {
                break;
            }

            // Извлекаем полный аудио-пакет и удаляем его из буфера
            QByteArray audioPacket = buffer.left(TOTAL_AUDIO_SIZE);
            buffer.remove(0, TOTAL_AUDIO_SIZE);

            // Рассылаем аудио всем слушателям в этой комнате
            QString roomCode = socketToRoom.value(socket);
            if (!roomCode.isEmpty()) {
                for (const ClientInfo &client : rooms[roomCode].clients) {
                    // Не отправляем аудио обратно самому хосту
                    if (client.socket && client.socket != socket && client.socket->isOpen()) {
                        client.socket->write(audioPacket);
                        client.socket->flush();
                    }
                }
            }
            continue;
        }

        // Обработка JSON-сообщения
        int jsonStart = buffer.indexOf('{');
        if (jsonStart < 0) {
            if (buffer.size() > 10000) {
                qDebug() << "⚠️ Обнаружен мусор в буфере, очистка.";
                buffer.clear();
            }
            break;
        }

        if (jsonStart > 0) {
            buffer.remove(0, jsonStart);
        }

        int jsonEnd = buffer.indexOf('}', 0);
        if (jsonEnd < 0) {
            break;
        }

        // Извлечение JSON и удаление его из буфера
        QByteArray jsonData = buffer.left(jsonEnd + 1);
        buffer.remove(0, jsonEnd + 1);

        // Парсинг JSON
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
        
        if (parseError.error != QJsonParseError::NoError) {
            qDebug() << "⚠️ Ошибка парсинга JSON:" << parseError.errorString();
            continue;
        }
        
        QJsonObject json = doc.object();
        QString action = json["action"].toString();
        
        if (action == "CREATE_ROOM") {
            QString roomName = json["roomName"].toString();
            QString hostName = json["hostName"].toString();
            
            roomCounter++;
            QString roomCode = QString("%1").arg(roomCounter, 4, 10, QChar('0'));
            
            RoomInfo room;
            room.roomName = roomName;
            room.hostName = hostName;
            rooms[roomCode] = room;
            
            ClientInfo hostClient;
            hostClient.socket = socket;
            hostClient.name = hostName;
            hostClient.roomCode = roomCode;
            hostClient.isHost = true;
            rooms[roomCode].clients.append(hostClient);
            
            socketToRoom[socket] = roomCode;
            socketToName[socket] = hostName;
            
            QJsonObject response;
            response["action"] = "ROOM_CREATED";
            response["roomCode"] = roomCode;
            response["roomName"] = roomName;
            response["hostName"] = hostName;
            sendMessage(socket, response);
            
            emit roomCreated(roomCode, roomName);
            qDebug() << "🏠 Создана комната:" << roomCode << "(" << roomName << ") хост:" << hostName;
        }
        else if (action == "JOIN_ROOM") {
            QString roomCode = json["roomCode"].toString();
            QString clientName = json["clientName"].toString();
            
            if (rooms.contains(roomCode)) {
                ClientInfo client;
                client.socket = socket;
                client.name = clientName;
                client.roomCode = roomCode;
                client.isHost = false;
                
                rooms[roomCode].clients.append(client);
                socketToRoom[socket] = roomCode;
                socketToName[socket] = clientName;
                
                QJsonObject response;
                response["action"] = "JOIN_SUCCESS";
                response["roomCode"] = roomCode;
                response["roomName"] = rooms[roomCode].roomName;
                response["hostName"] = rooms[roomCode].hostName;
                sendMessage(socket, response);
                
                QJsonObject update;
                update["action"] = "LISTENER_JOINED";
                update["clientName"] = clientName;
                broadcastToRoom(roomCode, update, socket);
                
                emit clientJoined(roomCode, clientName);
                qDebug() << "👤 Слушатель" << clientName << "вошёл в комнату" << roomCode;
            } else {
                QJsonObject response;
                response["action"] = "JOIN_FAILED";
                response["message"] = "Комната не найдена";
                sendMessage(socket, response);
            }
        }
        else if (action == "HOST_SPEAKING") {
            QString roomCode = socketToRoom.value(socket);
            if (!roomCode.isEmpty()) {
                bool status = json["status"].toBool();
                
                QJsonObject update;
                update["action"] = "HOST_SPEAKING";
                update["status"] = status;

                broadcastToRoom(roomCode, update, nullptr);
                
                if (status) {
                    qDebug() << "🎙️ Хост в комнате" << roomCode << "начал говорить";
                } else {
                    qDebug() << "🔇 Хост в комнате" << roomCode << "замолчал";
                }
            }
        }
        else if (action == "CHAT_MESSAGE") {
            QString roomCode = socketToRoom.value(socket);
            if (!roomCode.isEmpty()) {
                // Рассылаем сообщение всем в комнате, кроме отправителя 
                QJsonObject update = json;
                broadcastToRoom(roomCode, update, socket);
                
                qDebug() << "💬 [СЕРВЕР] Чат: '" << json["text"].toString() 
                         << "' от " << json["sender"].toString() 
                         << " в комнате " << roomCode;
            }
        }
    }
}

void PodcastServer::onDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        removeClient(socket);
    }
}

void PodcastServer::removeClient(QTcpSocket *socket)
{
    if (!socketToRoom.contains(socket)) return;
    
    QString roomCode = socketToRoom[socket];
    QString clientName = socketToName[socket];
    
    auto &clients = rooms[roomCode].clients;
    for (int i = 0; i < clients.size(); ++i) {
        if (clients[i].socket == socket) {
            bool wasHost = clients[i].isHost;
            clients.removeAt(i);
            
            // Если вышел хост — удаляем всю комнату
            if (wasHost || clients.isEmpty()) {
                rooms.remove(roomCode);
                qDebug() << "🗑️ Удалена комната (вышел хост):" << roomCode;
            }
            break;
        }
    }
    
    socketToRoom.remove(socket);
    socketToName.remove(socket);
    
    qDebug() << "👋 Клиент отключился:" << clientName;
}

void PodcastServer::sendMessage(QTcpSocket *socket, const QJsonObject &message)
{
    if (socket && socket->isOpen()) {
        QJsonDocument doc(message);
        socket->write(doc.toJson(QJsonDocument::Compact));
        socket->flush();
    }
}

void PodcastServer::broadcastToRoom(const QString &roomCode, const QJsonObject &message, QTcpSocket *excludeSocket)
{
    if (!rooms.contains(roomCode)) return;
    
    QJsonDocument doc(message);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    for (const ClientInfo &client : rooms[roomCode].clients) {
        if (client.socket && client.socket != excludeSocket && client.socket->isOpen()) {
            client.socket->write(data);
            client.socket->flush();
        }
    }
}