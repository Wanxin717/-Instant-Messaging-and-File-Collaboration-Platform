#ifndef CLIENT_H
#define CLIENT_H
#include "protocol.h"
#include "reshandler.h"

#include<QTcpSocket>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:

    ~Client();
    void loadConfig();
    QString m_strIP;
    quint16 m_usPort;
    QString m_strRootPath;
    QTcpSocket socket;
    QString m_strLoginName;
    //2.创造静态成员函数
    static Client& getInstance();
    void receMsg();
    PDU* readMsg();
    void handMsg(PDU* pdu);
    ResHandler *rh;
    QByteArray buffer;
public slots:
    void  showConnect();
    void sendMsg(PDU* pdu);
private slots:
    void on_regist_PB_clicked();
    void on_login_PB_clicked();

private:
    Ui::Client *ui;
    //1.将构造函数私有，并删除拷贝构造与拷贝赋值运算符
    Client(QWidget *parent = nullptr);
    Client(const Client& instance)=delete ;
    Client operator=(const Client&)=delete ;
};
#endif // CLIENT_H
