#include "socketioclient.h"
#include <QObject>

SocketIOClient::SocketIOClient(QObject *parent) :
    QObject(parent)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &SocketIOClient::onConnected);
        connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &SocketIOClient::onTextMessageReceived);
}

void SocketIOClient::connectToServer(const QUrl &url,const QString &cookie)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url.toString() + "/socket.io/?EIO=4&transport=websocket"));
    request.setRawHeader("Cookie", cookie.toUtf8()); // Adicione o cookie ao cabeçalho
    m_webSocket.open(request);
}

void SocketIOClient::onConnected()
{
    sendHandshake();
}

void SocketIOClient::onTextMessageReceived(const QString &message)
{
    handlePacket(message);
}



/*

void SocketIOClient::socketDisconnected()
{
    qDebug() << "WebSocket disconnected";
}*/

void SocketIOClient::sendMessage(const QString &event, const QJsonValue &data)
{
    QJsonArray payload;
        payload.append(event);
        payload.append(data);

        QString packet = QString("42%1").arg(QString(QJsonDocument(payload).toJson(QJsonDocument::Compact)));
        qDebug() << "Sending message:" << packet;
        m_webSocket.sendTextMessage(packet);
}

void SocketIOClient::sendHandshake()
{
    m_webSocket.sendTextMessage("40");
}

void SocketIOClient::handlePacket(const QString &packet)
{
    QChar type = packet[0];
    QString data = packet.mid(1);

    qDebug() << packet;
    qDebug() << "Tipo: " << type.toLatin1();


    switch (type.toLatin1()) {
        case '0': // CONNECT
            qDebug() << "Emitting connected signal";
            emit connected();
            break;
        case '2': // PING
            m_webSocket.sendTextMessage(QString("3%1").arg(data)); // Respond with PONG
            break;
        case '3': // PONG
            // Handle PONG if necessary
            break;
        case '4': // MESSAGE
            {
                if (data.startsWith("2")) {
                    QString adjustedData = data.mid(1);
                    QJsonDocument doc = QJsonDocument::fromJson(adjustedData.toUtf8());
                    QJsonArray payload = doc.array();

                    qDebug() << "Parsed message payload:" << payload;

                    if (payload.size() >= 2) {
                        QString event = payload[0].toString();
                        QJsonValue eventData = payload[1];

                        qDebug() << "Emitting messageReceived signal";
                        emit messageReceived(event, eventData);
                    } else {
                        qDebug() << "Payload size is less than 2";
                    }
                } else {
                    qDebug() << "Unhandled message type";
                }
            }
            break;
        default:
            // Handle other packet types if necessary
            break;
    }
}