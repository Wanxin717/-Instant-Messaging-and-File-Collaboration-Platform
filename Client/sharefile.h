#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>

namespace Ui {
class ShareFile;
}

class ShareFile : public QWidget
{
    Q_OBJECT

public:
    explicit ShareFile(QWidget *parent = nullptr);
    ~ShareFile();
    void updateFriend_LW();//将好友名写到列表框中

private slots:
    void on_allSelecT_PB_clicked();

    void on_cancelSelect_PB_clicked();

private:
    Ui::ShareFile *ui;
};

#endif // SHAREFILE_H
