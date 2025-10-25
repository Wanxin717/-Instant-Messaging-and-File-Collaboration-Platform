#include "client.h"
#include "onlineuser.h"
#include "ui_onlineuser.h"

Onlineuser::Onlineuser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Onlineuser)
{
    ui->setupUi(this);
}

Onlineuser::~Onlineuser()
{
    delete ui;
}

void Onlineuser::updateUserList(QStringList userList)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(userList);
}


//通过双击可以获得目标用户的用户名(也就是传入的参数)
void Onlineuser::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strCarName=Client::getInstance().m_strLoginName;
    QString srtTarName=item->text();
    PDU* pdu=mkPDU();
    pdu->uiType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData,strCarName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,srtTarName.toStdString().c_str(),32);
    //发送函数在Client类中，因此访问需要通过类的单例
    Client::getInstance().sendMsg(pdu);
}
