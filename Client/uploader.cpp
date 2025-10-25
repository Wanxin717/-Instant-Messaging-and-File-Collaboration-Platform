#include "uploader.h"
#include<QDebug>
#include <QThread>
#include <qfile.h>

Uploader::Uploader(QString strUploadFilePath)
{
    qDebug()<<"strUploadFilePath"<<strUploadFilePath;
    m_strUploadFilePath = strUploadFilePath;
}

void Uploader::start()
{
    QThread* thread = new QThread;//新建线程
    this->moveToThread(thread);//将类当前对象与线程对象关联
    //线程开始调用上传文件函数
    connect(thread, &QThread::started, this, &Uploader::uploadFile);
    //函数完成，线程函数结束
    connect(this, &Uploader::finished, thread, &QThread::quit);
    //线程函数完成，延时释放
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();

}

void Uploader::uploadFile()
{
    QFile file(m_strUploadFilePath);
        if (!file.open(QIODevice::ReadOnly)) {
            emit errorMsgBox("上传文件：打开文件失败");
            emit finished();
            return;
        }
//通过信号发送pdu，防止在信号未传输时，下一次数据传入pdu导致覆盖，因此循环发送
        while (true) {
            PDU* pdu = mkPDU(4096);
            pdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST;
            qint64 ret = file.read(pdu->caMsg, 4096);
            if (ret == 0) {
                break;
            }
            if (ret < 0) {
                emit errorMsgBox("上传文件：读取文件失败");
                break;
            }
            pdu->uiMsgLen = ret;
            pdu->uiPDULen = ret + sizeof (PDU);
            emit uploadPDU(pdu);
        }
        file.close();
        emit finished();

}
