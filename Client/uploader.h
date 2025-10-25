#ifndef UPLOADER_H
#define UPLOADER_H

#include "protocol.h"

#include <QObject>

class Uploader : public QObject
{
    Q_OBJECT
public:
    //通过构造函数传参
    Uploader(QString strUploadFilePath);
    void start();//定义线程函数
    QString m_strUploadFilePath;
public slots:
    void uploadFile();

signals:
    void finished();//定义线程完成信号
    //弹窗控件只能在主线程使用，因此将需要弹窗的位置转换为信号，关联控件
    void errorMsgBox(QString strError);
    //socket只能在创建的线程使用，创建信号，调用主线程中的发送
    void uploadPDU(PDU* pdu);

};

#endif // UPLOADER_H
