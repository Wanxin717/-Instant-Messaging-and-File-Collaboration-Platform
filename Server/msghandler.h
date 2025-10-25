#ifndef MSGHANDLER_H
#define MSGHANDLER_H
#include "protocol.h"
#include <QFile>
#include<QString>
class MsgHandler
{
public:
    MsgHandler();
    QFile m_fUploadFile;//创建文件对象
    qint64 m_iReceiveSize;//创建已接收文件的大小
    qint64 m_iUploadFileSize;//上传文件的大小
    PDU* pdu;
    PDU *regist();
    PDU* login(QString &m_strLoginName);
    PDU* find();
    PDU* onlineuser();
    PDU* addFriend();
    PDU* addFriendAgree();
    PDU* flushFriend();
    PDU* delFriend();
    PDU* chat();
    PDU* mkdir();
    PDU* flushFile();
    PDU*moveFile();
    PDU* uploadFile();
    PDU* uploadFileData();
};

#endif // MSGHANDLER_H
