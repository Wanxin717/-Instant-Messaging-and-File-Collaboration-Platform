#include "client.h"
#include "index.h"
#include "reshandler.h"

#include <QMessageBox>

ResHandler::ResHandler()
{

}

void ResHandler::regist()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof (bool));
    if(ret){
        //通过单例模式可以直接访问到Client的唯一窗口
        QMessageBox::information(&Client::getInstance(),"提示","注册成功");
   }else{
        QMessageBox::warning(&Client::getInstance(),"提示","注册失败");
    }
}

void ResHandler::login()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof (bool));
    if(ret){
//            QMessageBox::information(this,"提示","登录成功");
        Index::getInstance().show();//将Index设置为单例，登录成功直接跳转页面
        Index::getInstance().setWindowTitle(QString("首页-%1").arg(Client::getInstance().m_strLoginName));//显示当前登录用户名
        Client::getInstance().hide();//将登录页面隐藏，只显示跳转后Index的页面
   }else{
         QMessageBox::warning(&Client::getInstance(),"提示","登录失败");
    }
}

void ResHandler::find()
{
    //接收服务器传来的目标好友名字
    char caName[32]={'\0'};
    memcpy(caName,pdu->caData,32);
    int ret;
    memcpy(&ret,pdu->caData+32,sizeof (int));
    qDebug()<<"findUder ret"<<ret;
    if(ret==2){
         QMessageBox::information(&Index::getInstance(),"提示",QString("%1不存在").arg(caName));
   }else if(ret==-1){
         QMessageBox::information(&Index::getInstance(),"提示","查询失败");
   }else if(ret==0){
        QMessageBox::information(&Index::getInstance(),"提示",QString("%1不在线").arg(caName));
   }else if(ret==1){
       int ret= QMessageBox::information
    (&Index::getInstance(),"提示",QString("%1在线").arg(caName),"添加用户","取消");//增加两个按钮
    qDebug()<<"ret"<<ret;//在这里直接打印点击添加用户，查看成功的返回值是0
    if(ret==0){
        //添加好友的请求
        QString strCarName=Client::getInstance().m_strLoginName;
        PDU* pdu=mkPDU();
        pdu->uiType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
        memcpy(pdu->caData,strCarName.toStdString().c_str(),32);
        memcpy(pdu->caData+32,caName,32);
        Client::getInstance().sendMsg(pdu);
        //发出请求后与在线用户添加好友方式一致
    }
  }
}

void ResHandler::onlineuser()
{
    //PDU* respdu=mkPDU(res.size()*32);
    //传入的大小就是uiMsg的大小
    int iSize=pdu->uiMsgLen/32;
    QStringList res;
    char caName[32]={'\0'};
    for(int i=0;i<iSize;i++){
        memcpy(caName,pdu->caMsg+i*32,32);
        res.append(caName);
    }
Index::getInstance().getFriend()->m_pOnlineUser->updateUserList(res);

}

void ResHandler::addFriend()
{
    int ret;
    memcpy(&ret,pdu->caData,sizeof (int));
    qDebug()<<"findUder ret"<<ret;
    if(ret==2){
         QMessageBox::information(&Index::getInstance(),"提示","该用户已经是好友");
   }else if(ret==-1){
         QMessageBox::information(&Index::getInstance(),"提示","查询失败");
   }else if(ret==0){
        QMessageBox::information(&Index::getInstance(),"提示","该用户不在线");
    }
}
//将数据发送给服务器
void ResHandler::addFriendResend()
{
    char caName[32]={'\0'};
    memcpy(caName,pdu->caData,32);
    //YES与NO在QT中是int方式存储的
    int ret=QMessageBox::question(&Index::getInstance(),"提示",QString("是否同意%1的添加好友请求？").arg(caName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    PDU* respdu=mkPDU();
    respdu->uiType=ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUEST;
    memcpy(respdu->caData,pdu->caData,64);
    Client::getInstance().sendMsg(respdu);
}

void ResHandler::addFriendAgree()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof (bool));
    if(ret){
//  会导致信号阻塞      QMessageBox::information(&Client::getInstance(),"提示","添加成功");
        Index::getInstance().getFriend()->flushFriend();
   }else{
        QMessageBox::warning(&Client::getInstance(),"提示","添加失败");
    }
}

void ResHandler::flushFriend()
{
    int iSize=pdu->uiMsgLen/32;
    QStringList res;
    char caName[32]={'\0'};
    for(int i=0;i<iSize;i++){
        memcpy(caName,pdu->caMsg+i*32,32);
        res.append(caName);
    }
    Index::getInstance().getFriend()->updateFriendList(res);
}

void ResHandler::delFriend()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof (bool));
    qDebug()<<"ret"<<ret;
    if(ret){
        //通过单例模式可以直接访问到Client的唯一窗口
//        QMessageBox::information(&Client::getInstance(),"提示","删除成功");
        Index::getInstance().getFriend()->flushFriend();
   }else{
        QMessageBox::warning(&Client::getInstance(),"提示","删除失败");
    }
}

void ResHandler::chat()
{
    Chat* c=Index::getInstance().getFriend()->m_pChat;
    if (!c) return;
    if(c->isHidden()){
        c->show();
    }
    qDebug()<<"显示成功";
    char caChatName[32]={'\0'};
    memcpy(caChatName,pdu->caData,32);
    c->m_strChatName=caChatName;//更新成员变量
    //设置窗口名字
    c->setWindowTitle(caChatName);
    //拼接
    c->addShow_TE(QString("%1：%2").arg(caChatName).arg(pdu->caMsg));
    qDebug()<<"拼接成功";
}

void ResHandler::mkdir()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof (bool));
    if(ret){
        QMessageBox::information(&Index::getInstance(),"提示","文件创建成功");
   }else{
        QMessageBox::warning(&Index::getInstance(),"提示","文件创建失败");
    }
}

//接收服务器传来的数据，再进行在File类中更新
void ResHandler::flushFile()
{
    int iCount = pdu->uiMsgLen/sizeof(FileInfo);//计算文件总数
        qDebug() << "iCount" << iCount;
        QList<FileInfo*> pFileList;//创建一个新对象，用于存储所有文件的信息·
        for(int i=0; i<iCount; i++) {
            FileInfo* pFileInfo = new FileInfo;//创建一个新的结构体，用于接收数据
            memcpy(pFileInfo, pdu->caMsg+i*sizeof(FileInfo), sizeof(FileInfo));
            pFileList.append(pFileInfo);//将结构体存放在列表中
        }
        Index::getInstance().getFile()->updateFileList(pFileList);
}
void ResHandler::moveFile()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof (bool));
    if(ret){
        Index::getInstance().getFriend()->flushFriend();
   }else{
        QMessageBox::warning(&Index::getInstance(),"提示","文件移动失败");
    }
}

void ResHandler::uploadFile()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof (bool));
    if(ret){
        Index::getInstance().getFile()->uploadFile();
   }else{
        QMessageBox::warning(&Index::getInstance(),"提示","上传文件，服务器出初始化失败");
    }
}
void ResHandler::uploadFileData()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if (ret) {
       Index::getInstance().getFile()->flushFile();
    } else {
        QMessageBox::warning(&Index::getInstance(), "提示", "上传文件失败");
    }
}


