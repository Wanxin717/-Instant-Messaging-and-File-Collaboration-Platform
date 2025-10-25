#include "index.h"
#include "sharefile.h"
#include "ui_sharefile.h"

ShareFile::ShareFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::updateFriend_LW()
{
    ui->listWidget->clear();//清空
    QListWidget* friendList = Index::getInstance().getFriend()->getFriend_LW();//获取在线好友
    for (int i=0; i<friendList->count();i++) {//循环遍历将朋友的列表框元素取出
        QListWidgetItem* pFriendItem = new QListWidgetItem(*friendList->item(i));
        //添加到分享文件的列表框中
        ui->listWidget->addItem(pFriendItem);
    }
}

void ShareFile::on_allSelecT_PB_clicked()
{
    //循环遍历列表框中的数据，并且都选中(true)
    for (int i=0; i<ui->listWidget->count(); i++) {
       ui->listWidget->item(i)->setSelected(true);
       }

}

void ShareFile::on_cancelSelect_PB_clicked()
{
    for (int i=0; i<ui->listWidget->count(); i++) {
        //(false)取消选中
        ui->listWidget->item(i)->setSelected(false);
        }
}
