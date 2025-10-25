#include "mytcpserver.h"
#include "server.h"
#include "ui_server.h"

#include <QDebug>
#include <QFile>

Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);

}

Server::~Server()
{
    delete ui;
}

void Server::loadConfig()
{
      QFile file(":/client.config.txt");
      if(file.open(QIODevice::ReadOnly)){
          QString strData=QString(file.readAll());
          QStringList strList=strData.split("\r\n");
          m_strIP=strList[0];
          m_usPort=strList[1].toUShort();
          m_strRootPath=strList[2];//读取存储用户文件的root文件
          qDebug()<<"loadConfig IP"<<m_strIP<<"port"<<m_usPort<<"m_strRootPath"<<m_strRootPath;
          file.close();
      }else{
    qDebug()<<"loadConfig 打开配置文件失败";
      }
}

Server &Server::getInstance()
{
    static Server instance;
    return instance;
}

