#ifndef CLIENTTASK_H
#define CLIENTTASK_H

#include "mytcpsocket.h"

#include <QObject>
#include <QRunnable>

class ClientTask : public QObject,public QRunnable
{
    Q_OBJECT
public:
    //通过构造函数传入
    ClientTask(MyTcpSocket *socket);
    void run() override;//重写run方法，在线程池中会自动运行
    MyTcpSocket *socket;
signals:

};

#endif // CLIENTTASK_H
