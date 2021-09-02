#include "widget.h"
#include <QApplication>
#include <tcpclient.h>
#include <tcpserver.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Widget w;
//    w.show();


    TCPServer server1(7000,3);  //启动一个线程监听7000端口用于接受车队传来的指令
    server1.start();

    TCPServer server2(7003,40);  //启动一个线程监听7003端口用于接受车队传来的答案，由于答案长度是不固定的，所以第二个参数只是暂时性设置，可能构造函数得重载
    server2.start();



    //-----------发送准备考试指令---------------
    TCPClient client1(QHostAddress::LocalHost,7001);
    client1.connectServer();
    std::cout << "发送准备考试指令..." << std::endl;
    char data[3] = {'\x02','\x30','\x03'};  //指令数组
    client1.writeData(data,3);

    //--------------等待准备就绪指令--------------------
    QByteArray signal;
    std::cout<< "等待准备就绪指令..."<<std::endl;
    while (!server1.readFixedData(signal));
    if ((signal.at(0) == '\x02') && (signal.at(1) == '\x32') && (signal.at(2) == '\x03'))
            std::cout << "收到准备就绪指令..." << std::endl;
    else {
            std::cout << "指令错误，退出程序" << std::endl;
            return 0;
    }

    //-----------发送开始测试指令---------------
    std::cout << "发送开始测试指令..." << std::endl;
    //data = new char[3];
    data[0] = '\x02'; data[1] = '\x31'; data[2] = '\x03';
    //data = {'\x02','\x30','\x03'};
    client1.writeData(data,3);

    TCPClient client2(QHostAddress::LocalHost,7002);
    client2.connectServer();
    //----------------------发送令牌--------------------

    //..................令牌发送中.......................
    char token1[12]= {'\x02','\x02','\x03','\x04','\x05','\x06','\x07','\x08','\x09','\x43','\x00','\x03'};
    char token2[12]={'\x02','\x22','\x33','\x44','\x55','\x66','\x77','\x88','\x99','\x50','\x00','\x03'};

    client2.writeData(token1,12);
    sleep(5);
    client2.writeData(token2,12);


    //----------------------接受令牌发送完毕指令--------------------

    //------------------发送令牌发送完成指令---------------
    std::cout << "发送令牌发送完成指令..." << std::endl;
    //data = new char[3];
    data[0] = '\x02'; data[1] = '\x33'; data[2] = '\x03';
    client1.writeData(data,3);

    //--------------等待答案发送完成指令-------------------
    std::cout<< "等待答案发送完成指令..."<<std::endl;
    while (!server1.readFixedData(signal));
    if ((signal.at(0) == '\x02') && (signal.at(1) == '\x35') && (signal.at(2) == '\x03'))
            std::cout << "收到答案发送完成指令." << std::endl;
    else {
            std::cout << "指令错误，退出程序" << std::endl;
            return 0;
    }

    return a.exec();
}
