#include "server.h"
#include"qperatedb.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server::getInstance();
    //6.主函数调用
    QperateDB::getInstance().connect();
    return a.exec();
}
