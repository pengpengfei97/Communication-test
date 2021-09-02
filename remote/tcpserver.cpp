#include <tcpserver.h>
#include <QDebug>
TCPServer::TCPServer(int port,int length,QObject *parent):QThread(parent)
{
    PORT = port;
    lengthOfOneReam = length;
}

TCPServer::~TCPServer() //析构函数
{
    m_tcpserver->close();
    m_tcpsocket->deleteLater();
    delete m_tcpserver;
    delete m_tcpsocket;
}

void TCPServer::run()
{
    m_tcpserver = new QTcpServer;
    m_tcpsocket = new QTcpSocket;

    //--------------监听和关联---------------------------

    m_tcpserver->listen(QHostAddress::Any,PORT);
    connect(m_tcpserver,SIGNAL(newConnection()),this,SLOT(newConnect()),Qt::DirectConnection);


    QThread::exec();    //开启该线程的事件循环，线程不会退出
}

void TCPServer::newConnect()
{
    std::cout<<"Local server has a new TCP connect at Port "<<PORT<<std::endl;
    m_tcpsocket = m_tcpserver->nextPendingConnection(); //使用tcpServer的nextPendingConnection()函数来获取已经建立的连接的Tcp套接字，可以用它来完成数据的发送和其他操作
    connect(m_tcpsocket,SIGNAL(readyRead()),this,SLOT(readMessage()),Qt::DirectConnection); //readyRead信号和自定义的readMessage槽函数连接，当socket有新的数据时（并不是每当）都会发送信号
    connect(m_tcpsocket,SIGNAL(disconnected()),m_tcpsocket,SLOT(deleteLater()),Qt::DirectConnection);   //关闭连接时删除套接字
}

void TCPServer::readMessage()
{
    std::cout<<"reading data..."<<std::endl;
    QByteArray datagram;  //创建一个QByteArray类型的对象datagram
    datagram = m_tcpsocket->readAll();  //把传入的数据全部读入datagram中
    Fixeddata += datagram;  //把datagram的数据加到Fixeddata后
    QByteArray temp;
    if(Fixeddata.length()>=lengthOfOneReam)  //如果Fixeddata的长度大于服务器端对象设置的最大传输长度
    {
        temp = Fixeddata.mid(0,lengthOfOneReam);  //从Fixeddata的第一个字开始读数据，读到设置的能接收的最大长度为止
        //Fixeddata.remove(0,Fixeddata.length());    //读完后将Fixeddata读取了的数据删除
        Fixeddata.remove(0,Fixeddata.length());
        lock.lockForWrite();
        FixeddataVector.push_back(temp);  //把修正后的的数据加入到修正数据向量中
        lock.unlock();
    }

    dataVector.push_back(datagram); //把未修正的数据加入到数据向量中
}

//int TCPServer::readData(char* &address,int & size)
//这个函数是用来读取未修正数据的
//{
//    if(dataVector.isEmpty())
//        return 0;
//    size = dataVector.at(0).size();
//    address = new char[size];
//    memcpy(address,dataVector.at(0).data(),size);
//    dataVector.pop_front();
//    return 1;
//}

int TCPServer::readFixedData(QByteArray &data) //这里是引用类型，可以直接改变参数的值，形参及实参
{
    if(FixeddataVector.isEmpty())
        return 0;
    lock.lockForRead();
    data = FixeddataVector.at(0);   //令data等于FixeddataVector的第一个数据
    lock.unlock();
    lock.lockForWrite();
    FixeddataVector.pop_front();    //删除已经读的指令数据
    lock.unlock();
    return 1;
}


