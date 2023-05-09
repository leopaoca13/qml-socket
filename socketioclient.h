#ifndef SOCKETIOCLIENT_H
#define SOCKETIOCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

class SocketIOClient : public QObject
{
    Q_OBJECT
public:
    explicit SocketIOClient(QObject *parent = nullptr);
    Q_INVOKABLE void connectToServer(const QUrl &url,const QString &cookie);
    Q_INVOKABLE void sendMessage(const QString &event, const QJsonValue &data);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &event, const QJsonValue &data);
    //void errorOccurred(QAbstractSocket::SocketError error);
    //void socketDisconnected();

private slots:
    void onConnected();
    void onTextMessageReceived(const QString &message);

private:
    QWebSocket m_webSocket;
    void sendHandshake();
    void handlePacket(const QString &packet);
};

#endif // SOCKETIOCLIENT_H