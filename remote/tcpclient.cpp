#include "tcpclient.h"

TCPClient::TCPClient(QHostAddress address,int port)  //定义构造函数
{
    Address = address;
    Port = port;
}

TCPClient::~TCPClient()     //定义析构函数
{
    delete m_tcpsocket;
}

void TCPClient::writeData(char *data,int size)
{
    //std::cout<<"send a data"<<std::endl;
    m_tcpsocket->write(data,size);  //第一个参数是写入的数据，第二个参数是最大写入字节数
    m_tcpsocket->waitForBytesWritten(); //这个函数是等待数据发送完毕再返回，但是里面没有设置参数
}

void TCPClient::connectServer()  //连接服务器函数
{
    m_tcpsocket = new QTcpSocket;
    m_tcpsocket->connectToHost(Address,Port);
    if(!m_tcpsocket->waitForConnected(5000000)) //等待5000s如果还没连接上，报错 好像库里面有报错的接口函数
        std::cout<<"TCP socket connect to server failed! Server Port:"<<Port<<std::endl;
    else std::cout<<"TCP socket connect to server succeed! Server Port:"<<Port<<std::endl;
}

void TCPClient::closeConnect()  //关闭连接函数 这个
{
    std::cout<<"Close connection ,PORT:"<<Port<<std::endl;
    m_tcpsocket->disconnectFromHost();
    if (m_tcpsocket->state() == QAbstractSocket::UnconnectedState || m_tcpsocket->waitForDisconnected()) delete m_tcpsocket;
}

//
