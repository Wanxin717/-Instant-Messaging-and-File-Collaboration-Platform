#include "qperatedb.h"
#include<QtDebug>
#include<QSqlError>
#include <QSqlQuery>
QperateDB::QperateDB(QObject *parent) : QObject(parent)
{
    //3.创建连接对象
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}

QperateDB::~QperateDB()
{
    m_db.close();//4.关闭数据库
}
//5.创建连接函数
void QperateDB::connect()
{
    m_db.setHostName("localhost");// 设置数据库服务器的主机地址
    m_db.setPort(3306);//端口号
    m_db.setDatabaseName("mydb2501");//数据库名称
    m_db.setUserName("root");//用户名
    m_db.setPassword("123456");//密码
    if(m_db.open()){
        qDebug()<<"数据库连接成功";
    }else{
        qDebug()<<"数据库连接失败"<<m_db.lastError().text();
    }

}

QperateDB &QperateDB::getInstance()
{
    static QperateDB instance;
    return instance;
}

bool QperateDB::handRegist(const char *caName, const char *caPwd)
{
     if(caName==NULL || caPwd==NULL){
         return false;
     }
     //%1占位符
     QString sql=QString("select * from user_info where name='%1'").arg(caName);
      qDebug()<<"查询用户是否存在"<<sql;
      //创建对象，查询该语句
      QSqlQuery q;
      //执行失败或者查到了当前记录(数据库已中存在这条数据)
      if(!q.exec(sql)||q.next()){
          return false;
      }
sql=QString ("insert into user_info(name,pwd) values('%1','%2')").arg(caName).arg(caPwd);
    qDebug()<<"插入用户"<<sql;
    return q.exec(sql);
}

bool QperateDB::handLogin(const char *caName, const char *caPwd)
{
    if(caName==NULL || caPwd==NULL){
        return false;
    }
    //%1占位符
    QString sql=QString("select * from user_info where name='%1'and pwd='%2'").arg(caName).arg(caPwd);
     qDebug()<<"查询用户是否存在"<<sql;
     //创建对象，查询该语句
     QSqlQuery q;
     //执行失败或者查到了当前记录(数据库已中存在这条数据)
     if(!q.exec(sql)||!q.next()){
         return false;
     }
sql=QString ("update user_info set online=1 where name='%1'and pwd='%2'").arg(caName).arg(caPwd);
   qDebug()<<"用户在线状态置为1"<<sql;
   return q.exec(sql);
}

void QperateDB::handOffline(const char *caName)
{
    if(caName==NULL){
        return;
    }
     QSqlQuery q;

QString sql=QString ("update user_info set online=0 where name='%1'").arg(caName);
   qDebug()<<"用户在线状态置为0"<<sql;
   q.exec(sql);
}

int QperateDB::handFindUser(const char *caName)
{
    if(caName==NULL){
        return -1;
    }
     QSqlQuery q;

QString sql=QString ("select online from user_info  where name='%1'").arg(caName);
   qDebug()<<"通过用户名查找online字段"<<sql;
   q.exec(sql);
   //如果有有效记录就返回字段
   if(q.next()){
       //字段括号内传入的是下标
      return q.value(0).toInt();//返回online字段记录的0或1
   }
   return 2;
}

QStringList QperateDB::handOnlineUser()
{
    QString sql=QString("select name from user_info where online=1");
    QSqlQuery q;
    QStringList res;
    q.exec(sql);
 //通过循环获取每条数据记录，并存放到到res中
    while(q.next()){
        res.append(q.value(0).toString());
    }
    return res;
}
//返回值是int是为了返回添加不上的原因
int QperateDB::handleAddFriend(const char *caCurName, const char *caTarName)
{
    if(caCurName==NULL||caTarName==NULL){
        return -1;
    }
    //R"()"可以直接写入复杂sql语句
    QString sql=QString(R"(select *from friend
                        where
                        (
                        user_id=(select id from user_info where name='%1')
                        and
                        friend_id=(select id from user_info where name='%2')
                        )
                        or
                        (
                        user_id=(select id from user_info where name='%2')
                        and
                        friend_id=(select id from user_info where name='%1')
                        );
)").arg(caCurName).arg(caTarName);
    qDebug()<<"查看是否是好友"<<sql;
    QSqlQuery q;
    q.exec(sql);
    if(q.next()){
        return 2;
    }
    sql=QString("select online from user_info  where name='%1'").arg(caCurName);
    qDebug()<<"查看目标用户是否在线"<<sql;
    q.exec(sql);
    if(q.next()){
        return q.value(0).toInt();
    }
    return -1;
}

bool QperateDB::handleAddFriendAgree(const char *caCurName, const char *caTarName)
{
    if(caCurName==NULL||caTarName==NULL){
        return false;
    }
    QString sql=QString(R"(
       insert into friend(user_id,friend_id)
       select ui.id,u2.id
       from user_info ui,user_info u2
       where ui.name='%1' and u2.name='%2'
    )").arg(caCurName).arg(caTarName);
    qDebug()<<"添加好友记录"<<sql;
    QSqlQuery q;
    //执行成功返回ture 失败返回false
    return q.exec(sql);
}

QStringList QperateDB::handFlushFriend(const char *caName)
{

    QStringList res;
    if(caName==NULL){
        return res;
    }
    QString sql=QString(R"(
      select name from user_info
      where id in
      (
      select user_id from friend where
      friend_id=(select id from user_info where name='%1')
      union
      select friend_id from friend where
      user_id=(select id from user_info where name='%1')
      )
      and online=1;
      )").arg(caName);
      QSqlQuery q;
      q.exec(sql);
      while(q.next()){
          res.append(q.value(0).toString());
      }
      return res;
}

bool QperateDB::handledelFriend(const char *caCurName, const char *caTarName)
{
    if(caCurName==NULL||caTarName==NULL){
        return false;
    }
    QString sql=QString(R"(select *from friend
                         where
                         (
                         user_id=(select id from user_info where name='%1')
                         and
                         friend_id=(select id from user_info where name='%2')
                         )
                         or
                         (
                         user_id=(select id from user_info where name='%2')
                         and
                         friend_id=(select id from user_info where name='%1')
                         );
 )").arg(caCurName).arg(caTarName);
     qDebug()<<"查看是否是好友"<<sql;
      QSqlQuery q;
      q.exec(sql);
      if(!q.next()){
          qDebug() << caCurName << "和" << caTarName << "不是好友，无需删除";
          return false;
      }
      sql=QString(R"(
                  delete from friend
                  where
                  (
                  user_id=(select id from user_info where name='%1')
                  and
                  friend_id=(select id from user_info where name='%2')
                  )
                  or
                  (
                  user_id=(select id from user_info where name='%2')
                  and
                  friend_id=(select id from user_info where name='%1')
                  );)").arg(caCurName).arg(caTarName);
      qDebug()<<"删除好友sql"<<sql;
      return q.exec(sql);
}

