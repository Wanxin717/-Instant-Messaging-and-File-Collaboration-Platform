#include "clienttask.h"
#include "mytcpsocket.h"

#include <QThread>

ClientTask::ClientTask(MyTcpSocket *socket) : socket(socket)
{

}
//构造socket的线程池，socket通过构造函数
void ClientTask::run()
{
    socket->moveToThread(QThread::currentThread());
}
