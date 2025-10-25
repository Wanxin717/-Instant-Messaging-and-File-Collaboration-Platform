#include "client.h"
#include "index.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //4.将其他对象修改为静态局部对象
    Client::getInstance().show();
//     Index i;
//     i.show();
    return a.exec();
}
