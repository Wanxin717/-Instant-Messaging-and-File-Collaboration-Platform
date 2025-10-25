#ifndef RESHANDLER_H
#define RESHANDLER_H

#include "protocol.h"



class ResHandler
{
public:
    ResHandler();
    PDU* pdu;
    void regist();
    void login();
    void find();
    void onlineuser();
    void addFriend();
    void addFriendResend();
    void addFriendAgree();
    void flushFriend();
    void delFriend();
    void chat();
    void mkdir();
    void flushFile();
    void moveFile();
    void uploadFile();
    void uploadFileData();
};

#endif // RESHANDLER_H
