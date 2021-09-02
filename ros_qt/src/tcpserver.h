#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer> //编写服务器端
#include <QTcpSocket>  //编写客户端
#include <QThread> //引入线程
#include <iostream>
#include <QVector>
#include <QReadWriteLock> //读写锁，数据读取控制

class TCPServer:public QThread  //创建一个继承自QThread类的TCPServer类
{
    Q_OBJECT
public:
    TCPServer(int,int,QObject *parent = 0);     //构造函数
    ~TCPServer();   //析构函数
    void run();     //重写run()函数，线程从run()函数开始运行
//    int readData(char*&,int &);     //数据读取函数
    int readFixedData(QByteArray &);    //用于在主函数中获取得到的数据
    bool isEmpty();  //检查FixeddataVector是否为空
    QReadWriteLock lock;    //读写锁，线程在进行数据读写的时候使用

public slots:
    void newConnect();      //新建连接槽函数，响应newconnection信号
    void readMessage();     //数据读取槽函数，响应readyRead()信号
private:
    QTcpServer *m_tcpserver;    //作为服务器端时的对象
    QTcpSocket *m_tcpsocket;    //作为客户端时的对象
    QByteArray Fixeddata;
    QVector<QByteArray> dataVector;
    QVector<QByteArray> FixeddataVector;
    //QVector<QByteArray> FixeddataVector1; //数据测试vector
    int connected;  //连接标识符，未使用
    int lengthOfOneReam;    //指令数据长度
    int PORT;   //端口号
};

#endif // TCPSERVER_H
