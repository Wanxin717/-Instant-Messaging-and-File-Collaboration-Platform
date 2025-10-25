#include "client.h"
#include "file.h"
#include "ui_file.h"
#include "uploader.h"

#include <QFileDialog>
#include <qinputdialog.h>
#include <qmessagebox.h>

File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    ui->setupUi(this);
    m_strUserPath = QString("%1/%2").arg(Client::getInstance().m_strRootPath).arg(Client::getInstance().m_strLoginName);
    m_strCurPath = m_strUserPath;
    flushFile();
    m_pShareFile = new ShareFile;//展示出来
}


File::~File()
{
    delete ui;
    delete m_pShareFile;
}


void File::on_mkDir_PB_clicked()
{
    //在当前窗口上创建一个输入框，getText获取文本
    //参数含义是，在当前的哪个窗口，弹窗的名字是什么，提示用户输入的内容是什么
    QString strNewDir = QInputDialog::getText(this, "提示", "新建文件夹名");
    if (strNewDir.toStdString().size() == 0 || strNewDir.toStdString().size() > 32) {
        QMessageBox::information(this, "提示", "文件夹名长度非法");
        return;
    }
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiType = ENUM_MSG_TYPE_MKDIR_REQUEST;
    memcpy(pdu->caData, strNewDir.toStdString().c_str(), 32);
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::flushFile()
{
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}


//更新列表框函数
void File::updateFileList(QList<FileInfo *> pFileList)
{
    //创建对象，遍历m_pFileList
    //在每次刷新前，释放原本的
    foreach (FileInfo* pFileInfo, m_pFileList) {
        delete pFileInfo;
    }
    //创建File类成员变量，记录显示的文件，用来方便后续操作
    m_pFileList = pFileList;
    ui->listWidget->clear();
    foreach (FileInfo* pFileInfo, pFileList) {
        //每次循环更新一个列表行
        QListWidgetItem* pItem = new QListWidgetItem;
        if (pFileInfo->iType == 0) {
            //给每一行都设置一个图标
            pItem->setIcon(QIcon(QPixmap(":/dir.png")));
        } else if (pFileInfo->iType == 1) {
            pItem->setIcon(QIcon(QPixmap(":/file.png")));
        }
        pItem->setText(pFileInfo->caName);
        ui->listWidget->addItem(pItem);
    }
}

void File::on_flushFile_PB_clicked()
{
    flushFile();
}

void File::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    //遍历文件夹列表（包含文件名和文件属性
    foreach (FileInfo* pFileInfo, m_pFileList) {
        //当前当前文件夹名存在，并且不是文件夹直接返回
           if (item->text() == pFileInfo->caName && pFileInfo->iType !=0) {
               return;
           }
       }
       m_strCurPath = m_strCurPath + "/" + item->text();//修改文件路径
       flushFile();
   }


void File::on_return_PB_clicked()
{
    //没有上一级文件夹
    if (m_strCurPath == m_strUserPath) {
            return;
        }
    //找到路径的最后一个/的下标
        int index = m_strCurPath.lastIndexOf('/');
        //将/后面的东西删除
        m_strCurPath.remove(index, m_strCurPath.size() - index);
        flushFile();

}

void File::on_mvFile_PB_clicked()
{
    //选择要移动的文件的逻辑
    //判断按钮上的文字是移动文件
    if (ui->mvFile_PB->text() == "移动文件") {
        //获取点击的当前选中的项，也就是要移动的文件
            QListWidgetItem* pItem = ui->listWidget->currentItem();
            //如果是没选中，直接返回
            if (pItem == NULL) {
                return;
            }
            //选中了移动的文件就提示用户选择移动的目的路径
            QMessageBox::information(this, "移动文件", "请选择要移动到的目录");
            //修改按钮上的文字，选择好要移动的文件，点击确认/取消
            ui->mvFile_PB->setText("确认/取消");
            //记录选择的文件名及它的当前路径，也就是原路径
            m_strMvName = pItem->text();
            m_strMvPath = m_strCurPath;
            return;
        }
    ui->mvFile_PB->setText("移动文件");
    //获取目的路径选中的项
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    //定义目的路径
    QString strTarPath;
    //没选中，就移动到当前路径
    if (pItem == NULL) {
        strTarPath = m_strCurPath;
    } else {
        //先更新路径，再判别是文件夹，是文件夹就不进入if判断，不是文件夹就进入判断更新路径
        strTarPath = m_strCurPath + "/" + pItem->text();
       //用户选中的项是否是一个存在的文件夹，先确定存在，才能确定类型
       foreach (FileInfo* pFileInfo, m_pFileList) {
           if (pItem->text() == pFileInfo->caName && pFileInfo->iType != 0) {
               strTarPath = m_strCurPath;
               break;
           }
       }
    //选中文件夹，移动的路径
    }
    int ret = QMessageBox::information(this, "移动文件", QString("是否确认移动到\n%1？").arg(strTarPath), "确认", "取消");
    qDebug()<<"ret"<<ret;
    if (ret != 0) {
        return;
    }
    //拼接完整的文件原路径和移动的路径
    strTarPath = strTarPath + "/" + m_strMvName;
    QString strSrcPath = m_strMvPath + "/" + m_strMvName;
    int iSrcPathLen = strSrcPath.toStdString().size();
    int iTarPathLen = strTarPath.toStdString().size();
    qDebug()<<"iSrcPathLen"<<iSrcPathLen;
    qDebug()<<"iTarPathLen"<<iTarPathLen;
    PDU* pdu = mkPDU(iSrcPathLen + iTarPathLen + 1);
    pdu->uiType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
    memcpy(pdu->caData, &iSrcPathLen, sizeof(int));
    memcpy(pdu->caData + sizeof(int), &iTarPathLen, sizeof(int));
    memcpy(pdu->caMsg, strSrcPath.toStdString().c_str(), iSrcPathLen);
    memcpy(pdu->caMsg + iSrcPathLen, strTarPath.toStdString().c_str(), iTarPathLen);
    Client::getInstance().sendMsg(pdu);
}

void File::on_uploadFile_PB_clicked()
{
    //文件弹窗返回值是文件路径，定义一个成员变量记录
    m_strUploadFilePath = QFileDialog::getOpenFileName();
        qDebug() << "m_strUploadFilePath" << m_strUploadFilePath;
        //截取文件名，index记录最后一个“/“的下标
        int index = m_strUploadFilePath.lastIndexOf("/");
        //从右边截取，参数是截取长度
        QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size() - index - 1);
        qDebug() << "strFileName" << strFileName;
        //创建对象，来访问上传文件的大小
        QFile file(m_strUploadFilePath);
        qint64 iFileSize = file.size();
        size_t pathLen = m_strCurPath.toStdString().size();
        PDU* pdu = mkPDU(pathLen + 1);  // 关键：+1 预留\0的空间，防止传入脏数据
        pdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caData, strFileName.toStdString().c_str(), 32);
        memcpy(pdu->caData + 32, &iFileSize, sizeof(qint64));
        memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), pathLen);
        pdu->caMsg[pathLen] = '\0';//pathLen位置添加\0
        Client::getInstance().sendMsg(pdu);
}

void File::uploadError(QString strError)
{
    QMessageBox::warning(this, "提示", strError);
}
void File::uploadFile()
{
    //创建新对象，调用Uploader类构造函数，将参数传入Uploader中
    Uploader* uploader = new Uploader(m_strUploadFilePath);
    connect(uploader, &Uploader::errorMsgBox, this, &File::uploadError);
    connect(uploader, &Uploader::uploadPDU, &Client::getInstance(), &Client::sendMsg);
    uploader->start();

}
//将分享文件的窗口展示出来，通过成员变量建立联系
void File::on_shareFile_PB_clicked()
{
    //获取点击分享的当前文件
    QListWidgetItem* pItem = ui->listWidget->currentItem();
        if (pItem == NULL) {
            return;
        }
        //创建一个新的成员变量，存储分享的文件的文件名
        m_strShareFileName = pItem->text();
        //调用更新列表框的函数
        m_pShareFile->updateFriend_LW();
        if (m_pShareFile->isHidden()) {
            m_pShareFile->show();
        }
}
