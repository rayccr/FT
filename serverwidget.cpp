#include "serverwidget.h"
#include "ui_serverwidget.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>

ServerWidget::ServerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerWidget)
{
    ui->setupUi(this);

    ui->buttonSelect->setEnabled(false);
    ui->buttonSend->setEnabled(false);

    tcpServer = new QTcpServer(this);

    tcpServer->listen(QHostAddress::Any, 20015);

    connect(tcpServer, &QTcpServer::newConnection, [=](){
        tcpSocket = tcpServer->nextPendingConnection();

        QString name = tcpSocket->peerName();
        QString ip = tcpSocket->peerAddress().toString();
        quint16 port = tcpSocket->peerPort();

        QString tmp = QString("[%1 %2:%3]").arg(name).arg(ip).arg(port);
        ui->textBrowser->setText(tmp + " 已链接");

        ui->buttonSelect->setEnabled(true);

        connect(tcpSocket, &QTcpSocket::readyRead, [=](){
            QByteArray arr = tcpSocket->readAll();

            if(QString(arr) == "file done"){
                // 对方全部接收
                ui->buttonSelect->setEnabled(true);
                ui->buttonSend->setEnabled(false);
                QMessageBox::information(this, "success", "文件发送完成");
            } else if(QString(arr) == "client close") {
                destory();
                ui->textBrowser->append(QString(tmp + " 断开链接"));
                ui->buttonSelect->setEnabled(false);
                ui->buttonSend->setEnabled(false);
                QMessageBox::information(this, "断开连接", "client已经断开");
            }

        });

    });

    connect(&myTimer, &QTimer::timeout, [=](){
        // 关闭定时器
        myTimer.stop();

        // 发送文件
        sendData();
    });
}

ServerWidget::~ServerWidget()
{
    delete ui;
}

void ServerWidget::on_buttonSelect_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open", "../");

    if(!filePath.isEmpty()){
        // 获取文件信息
        QFileInfo info(filePath);
        fileName = info.fileName();
        fileSize = info.size();
        sendSize = 0;

        // 打开文件为只读
        file.setFileName(filePath);

        bool flag = file.open(QIODevice::ReadOnly);
        if(!flag) {
            qDebug() << "文件打开失败";
        }

        ui->textBrowser->append(QString("文件路径: " + filePath));
        ui->buttonSelect->setEnabled(false);
        ui->buttonSend->setEnabled(true);

    } else {
        qDebug() << "文件路径选择失败";
    }

}


void ServerWidget::on_buttonSend_clicked()
{
    // 发送文件信息
    QString str = QString("%1##%2").arg(fileName).arg(fileSize);

    qint64 len = tcpSocket->write(str.toUtf8().data()); // 转为char*
    if(len > 0) {
        // 发送文件信息成功

        //发送真正的文件内容
        //与write距离太近  会产成粘包问题
        // 用定时器延迟解决粘包问题

        myTimer.start(20); // 间隔20ms触发timeout信号

    } else {
        qDebug() << "发送文件信息失败";
        destory();
    }
}

void ServerWidget::sendData()
{
    qint64 len = 0;

    do {
        char buf[4 * 1024] = {0};

        // 读内容
        len = file.read(buf, sizeof(buf)); // 读了多少文件内容
        len = tcpSocket->write(buf, len); // 返回已经发送多少

        sendSize += len; // 已经发送多少文件内容

    } while(len > 0);

    if(sendSize == fileSize){
        file.close();
        sendSize = 0;
    }

}

void ServerWidget::destory()
{
    if(file.isOpen()) file.close();
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
}
