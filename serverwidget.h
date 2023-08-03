#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class ServerWidget; }
QT_END_NAMESPACE

class ServerWidget : public QWidget
{
    Q_OBJECT

public:
    ServerWidget(QWidget *parent = nullptr);
    ~ServerWidget();

    void sendData();

    void destory();


private slots:
    void on_buttonSelect_clicked();

    void on_buttonSend_clicked();

private:
    Ui::ServerWidget *ui;

    QTcpServer * tcpServer; // 监听套接字
    QTcpSocket * tcpSocket; // 通讯套接字

    QString fileName;
    qint64 fileSize; // 文件大小
    qint64 sendSize; // 已发送文件大小
    QFile file;

    QTimer myTimer;
};
#endif // SERVERWIDGET_H
