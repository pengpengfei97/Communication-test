#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <iostream>
#include <QVector>
#include <QReadWriteLock>

class TCPServer:public QThread  //创建一个继承自QThread类的TCPServer类
{
    Q_OBJECT
public:
    TCPServer(int,int,QObject *parent = 0);     //构造函数
    ~TCPServer();   //析构函数
    void run();     //重写run()函数，线程从run()函数开始运行
//    int readData(char*&,int &);     //数据读取函数
    int readFixedData(QByteArray &);
    QReadWriteLock lock;    //读写锁

public slots:
    void newConnect();
    void readMessage();
private:
    QTcpServer *m_tcpserver;    //作为服务器端时的对象
    QTcpSocket *m_tcpsocket;    //作为客户端时的对象
    QByteArray Fixeddata;       //
    QVector<QByteArray> dataVector;
    QVector<QByteArray> FixeddataVector;
    int connected;  //连接标识符
    int lengthOfOneReam;    //指令数据长度
    int PORT;
};

#endif // TCPSERVER_H
