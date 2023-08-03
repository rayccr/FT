#include "clientwidget.h"
#include "ui_clientwidget.h"
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QProcess>
#include <QStandardPaths>

ClientWidget::ClientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientWidget)
{
    ui->setupUi(this);

    tcpSocket = new QTcpSocket(this);

    isStart = true;

    ui->progressBar->setValue(0);

    //创建temp-path文件夹
    QString dir_str ="./DOWNLOAD/";
    // 检查目录是否存在，若不存在则新建
    QDir dir;
    if(!dir.exists(dir_str)) {
        bool res = dir.mkpath(dir_str);
    }

    connect(tcpSocket, &QTcpSocket::connected, [=](){
        qDebug() << "TCP Connected!";
        ui->buttonConnect->setEnabled(false);
        QMessageBox::information(this, "success", "链接成功");
   });

    connect(tcpSocket, &QTcpSocket::readyRead, [=](){
        QByteArray array = tcpSocket->readAll();

        // 发过来的是文件信息的开头
        if(isStart) {

            isStart = false;

            // 接收文件信息并拆包
            // hello##1024 拆包 section
            fileName = QString(array).section("##", 0, 0);
            fileSize = QString(array).section("##", 1, 1).toInt();

            recvSize = 0;

            file.setFileName("./DOWNLOAD/" + fileName);

            bool isOk = file.open(QIODevice::WriteOnly);

            if(!isOk){
                qDebug() << "文件打开失败";
                tcpSocket->disconnectFromHost();
                tcpSocket->close();
            }

            // 初始化进度条
            ui->progressBar->setMinimum(0);
            ui->progressBar->setMaximum(fileSize / 1024); // qint64 -> int
            ui->progressBar->setValue(0);

        } else {
            // 发送过来的是文件内容
            qint64 len = file.write(array); // 写入且获取写入多少

            recvSize += len;

            // 更新进度条
            ui->progressBar->setValue(recvSize / 1024);

            if(recvSize == fileSize) {
                file.close();
                QMessageBox::information(this, "接收文件", "接收文件成功");
                tcpSocket->write("file done");

                isStart = true;
                // 断开连接
//                tcpSocket->disconnectFromHost();
//                tcpSocket->close();
            }
        }
    });

}

ClientWidget::~ClientWidget()
{
    delete ui;
}

void ClientWidget::on_buttonConnect_clicked()
{
    QString ip = ui->lineEditIP->text();
    quint16 port = ui->lineEditPort->text().toInt();
    tcpSocket->connectToHost(ip, port);
}

void ClientWidget::closeEvent(QCloseEvent *event)
{
    qDebug() << "Window closed";
    tcpSocket->write("client close");

    // 断开连接
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
    QWidget::closeEvent(event);
}

void ClientWidget::on_pushButton_clicked()
{
    QString path = "./DOWNLOAD";
    const QString explorer = "explorer";
    QStringList param;
    if(!QFileInfo(path).isDir()){
        param<<QLatin1String("/select,");
    }
    param<<QDir::toNativeSeparators(path);

    QProcess::startDetached(explorer,param);
}

