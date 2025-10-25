#include "msghandler.h"
#include "mytcpserver.h"
#include "mytcpsocket.h"
#include "qperatedb.h"
#include "server.h"
#include <QDir>
#include<stdlib.h>
MsgHandler::MsgHandler()
{

}

PDU *MsgHandler::regist()
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    char caPwd[32] = {'\0'};
    memcpy(caPwd, pdu->caData+32, 32);

    bool ret = QperateDB::getInstance().handRegist(caName, caPwd);
    qDebug() << "regist ret" << ret;
    if (ret) {
        QDir dir;
        bool mkRet = dir.mkdir(QString("%1/%2").arg(Server::getInstance().m_strRootPath).arg(caName));
        qDebug() << "mkdir ret" << mkRet;
    }
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, &ret, sizeof(bool));
    respdu->uiType = ENUM_MSG_TYPE_REGIST_RESPOND;
    return respdu;
}


PDU *MsgHandler::login(QString &m_strLoginName)
{
    char caName[32]={'\0'};//初始化为\0，传入后多余位置是\0，保证传入的是有效字符
    memcpy(caName,pdu->caData,32);
    char caPwd[32]={'\0'};
    memcpy(caPwd,pdu->caData+32,32);
    //通过单例对象调用
    bool ret=QperateDB::getInstance().handLogin(caName,caPwd);
    qDebug()<<"ret"<<ret;
    if(ret){
         m_strLoginName=caName;
    }
    PDU* respdu=mkPDU();
    memcpy(respdu->caData,&ret,sizeof (bool));
    respdu->uiType=ENUM_MSG_TYPE_LOGIN_RESPOND;//respdu是服务器发射给客户端的响应信号
    //当前socket对象，将respdu发送给客户端
    return respdu;
}



PDU *MsgHandler::find()
{
    char caName[32]={'\0'};//初始化为\0，传入后多余位置是\0，保证传入的是有效字符
    memcpy(caName,pdu->caData,32);
    //通过单例对象调用
    int ret=QperateDB::getInstance().handFindUser(caName);
    qDebug()<<"handFindUser ret"<<ret;
    PDU* respdu=mkPDU();
    memcpy(respdu->caData,caName,32);//将caName（添加好友的目标名）传入respdu中
    memcpy(respdu->caData+32,&ret,sizeof (int));
    respdu->uiType=ENUM_MSG_TYPE_FIND_RESPOND;//respdu是服务器发射给客户端的响应信号
    //当前socket对象，将respdu发送给客户端
    return respdu;
}

PDU *MsgHandler::onlineuser()
{
    QStringList res=QperateDB::getInstance().handOnlineUser();
    qDebug()<<"onlineuser res.size"<<res.size();//获取在线一共有多少个用户
    PDU* respdu=mkPDU(res.size()*32);//设置一个用户名长度为32
    for(int i=0;i<res.size();i++){
       memcpy(respdu->caMsg+i*32,res[i].toStdString().c_str(),32);
    }
    respdu->uiType=ENUM_MSG_TYPE_ONLINE_USER_RESPOND;
    return respdu;
}

PDU *MsgHandler::addFriend()
{
    char caCurName[32]={'\0'};
    memcpy(caCurName,pdu->caData,32);
    char caTarName[32]={'\0'};
    memcpy(caTarName,pdu->caData+32,32);
    int ret=QperateDB::getInstance().handleAddFriend(caCurName,caTarName);
    qDebug()<<"addFriend ret"<<ret;
    if(ret==1){
        MyTcpServer::getInstance().resend(caTarName,pdu);
    }
    PDU* respdu=mkPDU();
    memcpy(respdu->caData,&ret,sizeof (ret));
    respdu->uiType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
    return respdu;
}

PDU *MsgHandler::addFriendAgree()
{
    char caCurName[32]={'\0'};
    memcpy(caCurName,pdu->caData,32);
    char caTarName[32]={'\0'};
    memcpy(caTarName,pdu->caData+32,32);
    bool ret=QperateDB::getInstance().handleAddFriendAgree(caCurName,caTarName);
    qDebug()<<"addFriendAgree ret"<<ret;
    PDU* respdu=mkPDU();
    memcpy(respdu->caData,&ret,sizeof (ret));
    respdu->uiType=ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPOND;
    MyTcpServer::getInstance().resend(caCurName,respdu);
    return respdu;
}

PDU *MsgHandler::flushFriend()
{
    char caCurName[32]={'\0'};
    memcpy(caCurName,pdu->caData,32);
    QStringList res=QperateDB::getInstance().handFlushFriend(caCurName);
    qDebug()<<"flushFriend res.size"<<res.size();
    PDU* respdu=mkPDU(res.size()*32);
    for(int i=0;i<res.size();i++){
       memcpy(respdu->caMsg+i*32,res[i].toStdString().c_str(),32);
    }
    respdu->uiType=ENUM_MSG_TYPE_FLUSH_FRIENDD_RESPOND;
    return respdu;

}

PDU *MsgHandler::delFriend()
{
    char caCurName[32]={'\0'};
    memcpy(caCurName,pdu->caData,32);
    char caTarName[32]={'\0'};
    memcpy(caTarName,pdu->caData+32,32);
    bool ret=QperateDB::getInstance().handledelFriend(caCurName,caTarName);
    qDebug()<<"delFriend ret"<<ret;
    PDU* respdu=mkPDU();
    memcpy(respdu->caData,&ret,sizeof (ret));
    respdu->uiType=ENUM_MSG_TYPE_DEL_FRIEND_RESPOND;
    MyTcpServer::getInstance().resend(caCurName,respdu);
    return respdu;
}

PDU *MsgHandler::chat()
{
    //获取目标用户名字，转发给目标用户
    char caTarName[32]={'\0'};
    memcpy(caTarName,pdu->caData+32,32);
    qDebug()<<"目标用户名"<<caTarName;
    MyTcpServer::getInstance().resend(caTarName,pdu);
    return NULL;
}

PDU *MsgHandler::mkdir()
{
    QDir dir;
    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(pdu->caData);
    qDebug() << "mkdir path" << strPath;
    bool ret;
    if (dir.exists(strPath)) {
        ret = false;
        qDebug() << "要创建的文件夹名已存在";
    }
    ret = dir.mkdir(QString("%1/%2").arg(pdu->caMsg).arg(pdu->caData));
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, &ret, sizeof(ret));
    respdu->uiType = ENUM_MSG_TYPE_MKDIR_RESPOND;
    return respdu;
}
PDU *MsgHandler::flushFile()
{
    //获取当前文件地址
    qDebug() << "flushFile path" << pdu->caMsg;
    QDir dir(pdu->caMsg);//创建对象，来调用该地址的文件
    QFileInfoList fileInfoList = dir.entryInfoList();//获取文件信息，一个文件，一条信息
    int iFileCount = fileInfoList.size();//获取文件数量

    PDU* respdu = mkPDU((iFileCount-2)*sizeof(FileInfo));//构建pdu 大小是文件数量*结构体大小
    respdu->uiType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
    for (int i=0,j=0; i<iFileCount; i++) {
        //将当前目录和父目录隐藏
        if(fileInfoList[i].fileName()=="."||fileInfoList[i].fileName()==".."){
            continue;
        }
        //循环遍历，将结构体放在caMsg中，设置文件偏移
        FileInfo* pFileInfo = (FileInfo*)respdu->caMsg+j++;
        //将文件名，放入在结构体中
        memcpy(pFileInfo->caName, fileInfoList[i].fileName().toStdString().c_str(), 32);
        //判断如果是目录类型设置为0
        if (fileInfoList[i].isDir()) {
            pFileInfo->iType = 0;
        } else {
            pFileInfo->iType = 1;
        }
        qDebug() << "pFileInfo->caName" << pFileInfo->caName;
    }
    return respdu;
}


PDU *MsgHandler::moveFile()
{
    int srcLen = 0;
    int tarLen = 0;
    memcpy(&srcLen, pdu->caData, sizeof(int));
    memcpy(&tarLen, pdu->caData+sizeof(int), sizeof(int));

    qDebug()<<"srcLen"<<srcLen;
     qDebug()<<"tarLen"<<tarLen;
     //创建两个新的字符数组
    char* pSrcPath = new char[srcLen+1];
    char* pTarPath = new char[tarLen+1];
    memset(pSrcPath, '\0', srcLen+1);//将字符数组初始化为\0
    memset(pTarPath, '\0', tarLen+1);
    memcpy(pSrcPath, pdu->caMsg, srcLen);
    memcpy(pTarPath, pdu->caMsg+srcLen, tarLen);
    qDebug() << "pSrcPath:" << pSrcPath
             << "pDestPath:" << pTarPath;

    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
    bool ret = QFile::rename(pSrcPath, pTarPath);//修改路径
    memcpy(respdu->caData, &ret, sizeof(bool));
    delete[] pSrcPath;
    delete[] pTarPath;
    pSrcPath = NULL;
    pTarPath = NULL;
    return respdu;
}
PDU *MsgHandler::uploadFile()
{
    m_iReceiveSize = 0;
    char caFileName[32] = {'\0'};
    memcpy(caFileName, pdu->caData, 32);
    memcpy(&m_iUploadFileSize, pdu->caData+32, sizeof(qint64));
    //将上传文件的目的路径拼接出来
    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(caFileName);
    //指定处理文件的目录
    m_fUploadFile.setFileName(strPath);
    //只写方式，文件不存在会创建并打开，文件存在清空并打开
    bool ret = m_fUploadFile.open(QIODevice::WriteOnly);
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, &ret, sizeof(ret));
    respdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
    return respdu;
}
PDU *MsgHandler::uploadFileData()
{
    //通过文件对象，将数据写入文件中，并更新接收数据长度
    m_fUploadFile.write(pdu->caMsg, pdu->uiMsgLen);
    m_iReceiveSize += pdu->uiMsgLen;
    //说明传入数据未接收完毕，返回NULL，直到数据完整接收
    if (m_iReceiveSize < m_iUploadFileSize) {
        return NULL;
    }
    m_fUploadFile.close();
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPOND;
    bool ret = m_iReceiveSize == m_iUploadFileSize;
    memcpy(respdu->caData, &ret, sizeof(ret));
    return respdu;
}





