#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QFile>
#include <QCloseEvent>

namespace Ui {
class ClientWidget;
}

class ClientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClientWidget(QWidget *parent = nullptr);
    ~ClientWidget();

    void closeEvent(QCloseEvent *event);


private slots:
    void on_buttonConnect_clicked();


    void on_pushButton_clicked();

private:
    Ui::ClientWidget *ui;

    QTcpSocket *tcpSocket;
    bool isStart;

    QString fileName;
    qint64 fileSize; // 文件大小
    qint64 recvSize; // 接收到文件的大小

    QFile file;
};

#endif // CLIENTWIDGET_H
