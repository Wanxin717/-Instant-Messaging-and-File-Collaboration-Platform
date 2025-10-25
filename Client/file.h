#ifndef FILE_H
#define FILE_H

#include "protocol.h"
#include "sharefile.h"

#include <QListWidget>
#include <QWidget>

namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    explicit File(QWidget *parent = nullptr);
    ~File();
    QString m_strUserPath;//用户路径
    QString m_strCurPath;//当前路径
    void flushFile();
    void updateFileList(QList<FileInfo*> pFileList);
    QList<FileInfo*> m_pFileList;
    QString m_strMvName;
    QString m_strMvPath;
    QString m_strUploadFilePath;
    QString m_strShareFileName;//存下分享文件的文件名
    void uploadFile();
    ShareFile *m_pShareFile;//创建联系的成员函数
private slots:
    void on_mkDir_PB_clicked();
    void on_flushFile_PB_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_return_PB_clicked();
    void on_mvFile_PB_clicked();
    void on_uploadFile_PB_clicked();
    void on_shareFile_PB_clicked();

public slots:
    //因为弹窗是文件类的
    void uploadError(QString strError);//上传文件错误槽函数，参数是错误信息
private:
    Ui::File *ui;
};

#endif // FILE_H
