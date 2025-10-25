#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "msghandler.h"
#include "protocol.h"

#include <QObject>
#include <QTcpSocket>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString m_strLoginName;
    void sendMsg(PDU* respdu);
    PDU* readMsg();
    PDU* handleMsg(PDU *pdu);
    MsgHandler* mh;
    ~MyTcpSocket();
    QByteArray buffer;
public slots:
    void receMsg();
    void clientOffline();
};

#endif // MYTCPSOCKET_H
