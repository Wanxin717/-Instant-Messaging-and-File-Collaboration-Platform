#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "mytcpsocket.h"

#include <QObject>
#include <QTcpServer>
#include <QThreadPool>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    static MyTcpServer &getInstance();
    void incomingConnection(qintptr handle) override;//监听函数
    QList<MyTcpSocket*> m_tcpSocketList;
    void deleteSocket(MyTcpSocket *mysocket);
    //转发函数 参数是目标名以及转发的内容(PDU)
    void resend(char* strName,PDU* pdu);
    QThreadPool threadPool;//创建一个线程池对象，指定最大限制是多少
private:
    MyTcpServer();
    MyTcpServer (const MyTcpServer& instance)=delete ;
    MyTcpServer& operator=(const MyTcpServer&)=delete ;

};

#endif // MYTCPSERVER_H
