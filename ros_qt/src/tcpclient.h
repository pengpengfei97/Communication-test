#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QtNetwork>
#include <QTcpSocket>
#include <iostream>

class TCPClient
{
public:
    TCPClient(QHostAddress,int);  //构造函数
    ~TCPClient();  //析构函数
    void writeData(char *,int);  //写数据的函数
    void connectServer();  //连接服务器函数
    void closeConnect();  //关闭连接函数

private:
    QTcpSocket *m_tcpsocket;   //tcpsocket对象
    QHostAddress Address;   //主机地址
    int Port;   //端口号
};
#endif // TCPCLIENT_H
