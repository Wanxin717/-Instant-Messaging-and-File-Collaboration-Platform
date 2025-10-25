#ifndef ONLINEUSER_H
#define ONLINEUSER_H

#include <QListWidget>
#include <QWidget>

namespace Ui {
class Onlineuser;
}

class Onlineuser : public QWidget
{
    Q_OBJECT

public:
    explicit Onlineuser(QWidget *parent = nullptr);
    ~Onlineuser();
    void updateUserList(QStringList userList);

private slots:
    //头文件与cpp文件都是用的类，在.h文件中导入库
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::Onlineuser *ui;
};

#endif // ONLINEUSER_H
