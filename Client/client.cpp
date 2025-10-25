#include "client.h"
#include "index.h"
#include "protocol.h"
#include "ui_client.h"
#include<QDebug>
#include <QFile>
#include <QHostAddress>
#include <QMessageBox>
#include<string.h>


Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    loadConfig();
//连接到服务器，没有返回值因，但是会发出成功信号
//connectToHost连接服务器，QHostAddress将字符串转化为网络地址对象
    socket.connectToHost(QHostAddress(m_strIP),m_usPort);
    QObject::connect(&socket, &QTcpSocket::readyRead, this, &Client::receMsg);
    QObject::connect(&socket,&QTcpSocket::connected,this,&Client::showConnect);//服务器连接到槽函数
//将信号连接给槽函数，因此可以查看连接状态
    rh=new ResHandler;
}

Client::~Client()
{
    delete ui;
    delete rh;
}

void Client::loadConfig(){
    QFile file(":/client.config.txt");//获取配置文件地址
    if(file.open(QIODevice::ReadOnly))//使用文件IO打卡配置文件 形式为只读
    {
        QString strData=QString(file.readAll());//读取文件中的内容，转换为字符串形式
        qDebug()<<"strData"<<strData;
        //按\r\n进行分割字符串，将IP与端口号分开
        QStringList strList=strData.split("\r\n");
        //获取第一个元素
        //将其声明为成员变量，使得其他的成员函数也可以访问
        m_strIP=strList[0];
        //获取端口号，转换为无符号短整型(16位)
        m_usPort=strList[1].toUShort();
        m_strRootPath=strList[2];
        qDebug()<<"loadConfig IP"<<m_strIP<<"port"<<m_usPort<<"m_strRootPath"<<m_strRootPath;
        file.close();

    }else{
        qDebug()<<"loadConfig 打开配置文件失败";
    }

}
//3.创造静态局部变量
Client &Client::getInstance()
{
    static Client instance;
    return instance;
}
PDU* Client::readMsg()
{
    qDebug()<<"receMsg接收消息长度"<<socket.bytesAvailable();
    uint uiPDULen=0;
    socket.read((char*)&uiPDULen,sizeof(uint));
    uint uiMsgLen=uiPDULen-sizeof(PDU);
    PDU* pdu=mkPDU(uiMsgLen);
    socket.read((char*)pdu+sizeof (uint),uiPDULen-sizeof (uint));
    qDebug() << "receMsg->uiPDULen" << pdu->uiPDULen
             << "pdu->uiMsgLen" << pdu->uiMsgLen
             << "pdu->uiType" << pdu->uiType
             << "pdu->caData" << pdu->caData
             << "pdu->caData+32" << pdu->caData + 32
             << "pdu->caMsg" << pdu->caMsg;
    return pdu;
}

void Client::handMsg(PDU *pdu)
{
    if(pdu==NULL){
        return;
    }
    rh->pdu=pdu;
    switch (pdu->uiType) {
    case ENUM_MSG_TYPE_REGIST_RESPOND:{
        rh->regist();
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:{
       rh->login();
        break;
    }
    case ENUM_MSG_TYPE_FIND_RESPOND:{
       rh->find();
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USER_RESPOND:{
        rh->onlineuser();
          break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
        rh->addFriend();
          break;
    }
        //tar处理来自服务器的请求
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        rh->addFriendResend();
          break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPOND:{
        rh->addFriendAgree();
          break;
    }     
   case ENUM_MSG_TYPE_FLUSH_FRIENDD_RESPOND:{
        rh->flushFriend();
          break;
    }
    case ENUM_MSG_TYPE_DEL_FRIEND_RESPOND:{
        rh->delFriend();
           break;
     }
    case ENUM_MSG_TYPE_CHAT_REQUEST:{
        rh->chat();
           break;
     }
    case ENUM_MSG_TYPE_MKDIR_RESPOND:{
        rh->mkdir();
           break;
     }
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:{
        rh->flushFile();
           break;
     }
    case ENUM_MSG_TYPE_MOVE_FILE_RESPOND:{
        rh->moveFile();
           break;
     }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:{
        rh->uploadFile();
           break;
     }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPOND:{
        rh->uploadFileData();
           break;
     }
    default:
        break;
    }
    free(pdu);
    pdu=NULL;
}
void Client::sendMsg(PDU* pdu)
{
    socket.write((char*)pdu,pdu->uiPDULen);
    qDebug() << "send msg pdu->uiPDULen" << pdu->uiPDULen
             << "pdu->uiMsgLen" << pdu->uiMsgLen
             << "pdu->uiType" << pdu->uiType
             << "pdu->caData" << pdu->caData
             << "pdu->caData+32" << pdu->caData + 32
             << "pdu->caMsg" << pdu->caMsg;
        free(pdu);
        pdu = NULL;
}
void Client::receMsg()
{
    PDU* pdu=readMsg();
    handMsg(pdu);

}

void Client::showConnect(){
    qDebug()<<"连接服务器成功";
}


void Client::on_regist_PB_clicked()
{
    QString strName=ui->nameLable->text();
    QString strPwd=ui->passwdLable->text();
    if(strName.isEmpty()||strName.toStdString().size()>32){
        QMessageBox::information(this,"提示","用户名长度非法");
        return;
    }
    if(strPwd.isEmpty()||strPwd.toStdString().size()>32){
        QMessageBox::information(this,"提示","密码长度非法");
        return;
    }

    PDU* pdu=mkPDU();//传入caData中，不需要传入长度参数
    pdu->uiType=ENUM_MSG_TYPE_REGIST_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
    sendMsg(pdu);
    }


void Client::on_login_PB_clicked()
{
    QString strName=ui->nameLable->text();
    QString strPwd=ui->passwdLable->text();
    if(strName.isEmpty()||strName.toStdString().size()>32){
        QMessageBox::information(this,"提示","用户名长度非法");
        return;
    }
    if(strPwd.isEmpty()||strPwd.toStdString().size()>32){
        QMessageBox::information(this,"提示","密码长度非法");
        return;
    }
    m_strLoginName=strName;
    PDU* pdu=mkPDU();//传入caData中，不需要传入长度参数
    pdu->uiType=ENUM_MSG_TYPE_LOGIN_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
    sendMsg(pdu);

}
