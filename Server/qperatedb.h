#ifndef QPERATEDB_H
#define QPERATEDB_H

#include <QObject>
#include <QSqlDatabase>

class QperateDB : public QObject
{
    Q_OBJECT
public:

    QSqlDatabase m_db;//1.创建连接数据库的对象
    ~QperateDB();//2.添加析构函数
    void connect();//5.连接数据库
    static QperateDB& getInstance();
    bool handRegist(const char *caName,const char* caPwd);
    bool handLogin(const char *caName,const char* caPwd);
    void handOffline(const char *caName);
    int  handFindUser(const char *caName);
    QStringList handOnlineUser();
    int handleAddFriend(const char *caCurName,const char* caTarName);
    bool handleAddFriendAgree(const char *caCurName,const char* caTarName);
    QStringList handFlushFriend(const char *caName);
    bool handledelFriend(const char *caCurName,const char* caTarName);

private:
    explicit QperateDB(QObject *parent = nullptr);
    QperateDB(const QperateDB& instance)=delete ;
    QperateDB &operator=(const QperateDB&)=delete ;
signals:

};

#endif // QPERATEDB_H
