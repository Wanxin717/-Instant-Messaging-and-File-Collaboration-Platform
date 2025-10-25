#ifndef FRIEND_H
#define FRIEND_H

#include "chat.h"
#include "onlineuser.h"

#include <QWidget>

namespace Ui {
class Friend;
}

class Friend : public QWidget
{
    Q_OBJECT

public:
    explicit Friend(QWidget *parent = nullptr);
    ~Friend();
    Onlineuser *m_pOnlineUser;
//通过Friend访问在线用户因此在Friend中创建一个Onlineuser类型的变量，并在构造函数中声明
    void flushFriend();
    void updateFriendList(QStringList friendList);
    Chat *m_pChat;
    QListWidget* getFriend_LW();//ui是私有的因此通过类中的函数获取
private slots:
    void on_findUser_PB_clicked();

    void on_onlineUser_PB_clicked();

    void on_flushFriend_PB_clicked();

    void on_delFriend_PB_clicked();

    void on_chat_PB_clicked();

private:
    Ui::Friend *ui;
};

#endif // FRIEND_H
