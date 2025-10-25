#include "mytcpserver.h"
#include "mytcpsocket.h"
#include "mytcpsocket.h"
#include"protocol.h"
#include "qperatedb.h"
MyTcpSocket::MyTcpSocket(QObject *parent) : QTcpSocket(parent)
{
    mh = new MsgHandler;
    connect(this, &QTcpSocket::readyRead, this, &MyTcpSocket::receMsg);
    connect(this, &QTcpSocket::disconnected, this, &MyTcpSocket::clientOffline);
}

void MyTcpSocket::clientOffline()
{
    QperateDB::getInstance().handOffline(m_strLoginName.toStdString().c_str());
    MyTcpServer::getInstance().deleteSocket(this);
}

void MyTcpSocket::sendMsg(PDU *respdu)
{
    if(respdu==NULL){
        return ;
    }
    this->write((char*)respdu,respdu->uiPDULen);
    qDebug() << "send msg pdu->uiPDULen" << respdu->uiPDULen
             << "pdu->uiMsgLen" << respdu->uiMsgLen
             << "pdu->uiType" << respdu->uiType
             << "pdu->caData" << respdu->caData
             << "pdu->caData+32" << respdu->caData + 32
             << "pdu->caMsg" << respdu->caMsg;
    free(respdu);
    respdu = NULL;
}

PDU *MyTcpSocket::readMsg()
{
    qDebug()<<"readMsg接收消息长度"<<this->bytesAvailable();
    //流式传输，防止粘包问题，先读取出长度(结构体的第一个成员变量)，存放在uiPDULen中
    uint uiPDULen=0;
    this->read((char*)&uiPDULen,sizeof(uint));
    //结构体不包含柔性数字大小
    uint uiMsgLen=uiPDULen-sizeof(PDU);
    PDU* pdu=mkPDU(uiMsgLen);
    //指针偏移，跳过uiPDULen，读取剩余内容
    this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    qDebug() << "send msg pdu->uiPDULen" << pdu->uiPDULen
             << "pdu->uiMsgLen" << pdu->uiMsgLen
             << "pdu->uiType" << pdu->uiType
             << "pdu->caData" << pdu->caData
             << "pdu->caData+32" << pdu->caData + 32
             << "pdu->caMsg" << pdu->caMsg;
    return pdu;
}

PDU *MyTcpSocket::handleMsg(PDU *pdu)
{
    if(pdu==NULL){
        return NULL;
    }
    PDU* respdu=NULL;
    mh->pdu=pdu;
    switch (pdu->uiType) {
    case ENUM_MSG_TYPE_REGIST_REQUEST:{
        respdu=mh->regist();
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:{
        respdu=mh->login(m_strLoginName);
        break;
    }
    case ENUM_MSG_TYPE_FIND_REQUEST:{
        respdu=mh->find();
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USER_REQUEST:{
        respdu=mh->onlineuser();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        respdu=mh->addFriend();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUEST:{
        respdu=mh->addFriendAgree();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:{
        respdu=mh->flushFriend();
        break;
    }
    case ENUM_MSG_TYPE_DEL_FRIEND_REQUEST:{
        respdu=mh->delFriend();
        break;
    }
    case ENUM_MSG_TYPE_CHAT_REQUEST:{
        respdu=mh->chat();
        break;
    }
    case ENUM_MSG_TYPE_MKDIR_REQUEST:{
        respdu=mh->mkdir();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:{
        respdu=mh->flushFile();
        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_REQUEST:{
        respdu=mh->moveFile();
        break;
    } 
    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:{
        respdu=mh->uploadFile();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST:{
        respdu=mh->uploadFileData();
        break;
    }

    default:
        break;
    }

    free(pdu);
    pdu=NULL;
    return respdu;
}

MyTcpSocket::~MyTcpSocket()
{
    delete mh;
}

void MyTcpSocket::receMsg()
{ 
     PDU* pdu=readMsg();
     PDU *respdu=handleMsg(pdu);
     sendMsg(respdu);

//     //定义buffer成员变量，全部未处理的数据
//     qDebug() << "recvMsg 接收消息长度" << this->bytesAvailable();
//     QByteArray data = this->readAll();
//     buffer.append(data);//建立数据缓冲区，将读取的数据放在缓冲区中
//     //判断是否够一个pdu
//     while (buffer.size() >= int(sizeof(PDU))) {
//         //通过data转成char* 类型，再转为PDU类型
//         PDU* pdu = (PDU*)buffer.data();
//         //缓冲区大小<pdu的总程度，说明数据不是完整的是半包
//         if (buffer.size() < int(pdu->uiPDULen)) {
//             break;
//         }
//         PDU* respdu = handleMsg(pdu);
//         sendMsg(respdu);
//         //将已经读取的东西移除
//         buffer.remove(0, pdu->uiPDULen);
//     }

}
