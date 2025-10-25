#include "clienttask.h"
#include "mytcpserver.h"
#include "qperatedb.h"
#include<QDebug>
MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

MyTcpServer::MyTcpServer()
{
    //设置线程池的最大限度 为8
    threadPool.setMaxThreadCount(8);
}


void MyTcpServer::incomingConnection(qintptr handle)
{
    qDebug() << "新客户端连接";
    MyTcpSocket* pSocket = new MyTcpSocket(this);
    pSocket->setSocketDescriptor(handle);
    m_tcpSocketList.append(pSocket);
    //每一个客服端连接，都新建一个socket，新建一个客户端任务对象
    ClientTask* task = new ClientTask(pSocket);
    //执行这个任务
    threadPool.start(task);

}


void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    m_tcpSocketList.removeOne(mysocket);
    mysocket->deleteLater();
    mysocket=NULL;
    for(int i=0;i<m_tcpSocketList.size();i++){
      qDebug()<<m_tcpSocketList[i]->m_strLoginName;
    }
}

void MyTcpServer::resend(char *strName, PDU *pdu)
{
    if(strName==NULL||pdu==NULL){
        return;
    }
    for(int i=0;i<m_tcpSocketList.size();i++){
        if(QString (strName)==m_tcpSocketList[i]->m_strLoginName)
        {
            m_tcpSocketList[i]->write((char*)pdu,pdu->uiPDULen);
            qDebug() << "resend msg pdu->uiPDULen" << pdu->uiPDULen
                     << "pdu->uiMsgLen" << pdu->uiMsgLen
                     << "pdu->uiType" << pdu->uiType
                     << "pdu->caData" << pdu->caData
                     << "pdu->caData+32" << pdu->caData + 32
                     << "pdu->caMsg" << pdu->caMsg;
            break;
        }
    }
}
