#include "client.h"
#include "friend.h"
#include "protocol.h"
#include "ui_friend.h"

#include <QInputDialog>
#include <QMessageBox>


Friend::Friend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
    m_pOnlineUser=new Onlineuser;
    m_pChat = new Chat;
}

Friend::~Friend()
{
    delete ui;
    delete m_pOnlineUser;//在析构函数中释放
    delete m_pChat;
}

void Friend::flushFriend()
{
    PDU* pdu=mkPDU();
    pdu->uiType=ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,Client::getInstance().m_strLoginName.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);
}

void Friend::updateFriendList(QStringList friendList)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(friendList);
}

QListWidget *Friend::getFriend_LW()
{
    return ui->listWidget;//返回值为friend的列表框
}

void Friend::on_findUser_PB_clicked()
{
    //定义输入弹窗，传入父类对象是当前的friend窗口
    QString strName=QInputDialog::getText(this,"查找用户","用户名：");
    if(strName.isEmpty())return ;
    if(strName.toStdString().size()>32||strName.size()==0){
        QMessageBox::information(this,"提示","用户名字非法");
        return;
    }
    PDU* pdu=mkPDU();
    pdu->uiType=ENUM_MSG_TYPE_FIND_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    //socket是Client中的成员变量可以通过单例模式进行访问
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_onlineUser_PB_clicked()
{
    if(m_pOnlineUser->isHidden()){
        m_pOnlineUser->show();//显示
    }
    //客户端只需要发送请求就可以
    PDU* pdu=mkPDU();
    pdu->uiType=ENUM_MSG_TYPE_ONLINE_USER_REQUEST;
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_flushFriend_PB_clicked()
{
    flushFriend();
}

void Friend::on_delFriend_PB_clicked()
{
    QListWidgetItem *pItem=ui->listWidget->currentItem();
    //通过当前列表窗口获取当前选中的项
    if(!pItem){
        return;
    }
    QString strDelName=pItem->text();//获取当前用户名
    int ret=QMessageBox::question(this,"提示",QString("是否同意删除好友%1").arg(strDelName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    PDU* pdu=mkPDU();
    pdu->uiType=ENUM_MSG_TYPE_DEL_FRIEND_REQUEST;
    memcpy(pdu->caData,Client::getInstance().m_strLoginName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strDelName.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);

}

void Friend::on_chat_PB_clicked()
{
    QListWidgetItem *pItem=ui->listWidget->currentItem();
    if(!pItem){
        return;
    }
    if(m_pChat->isHidden()){
        m_pChat->show();//显示
    }
    m_pChat->m_strChatName=pItem->text();//获取当前用户名
}
