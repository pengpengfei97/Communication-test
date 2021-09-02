//#include "widget.h"
#include <QApplication>
#include "tcpclient.h"
#include "tcpserver.h"
#include <iostream>
#include <token.h>

#define byte unsigned char


QByteArray selectFile(Token t)  //根据令牌类型选择文件
{
    QByteArray path;
    switch(t.typeOfToken){
    case 'C':
        path = "Raw-Camera-Timestamp.csv";
        break;
    case 'G':
        path = "Canbus-Gear.csv";
        break;
    case 'O':
        path = "Canbus-Odom.csv";
        break;
    case 'S':
        path = "Canbus-Steer.csv";
        break;
    case 'P':
        path = "Raw-GPS-withOffset.csv";
        break;
    case 'U':
        path = "LMU.csv";
        break;
    case 'L':
        path = "HDL32.pcap";
        break;
    case 'R':
        path = "URG.lms";
        break;
//    case 'T':
//        path = "Task.txt";
    }
    return path;
}
bool DecodeToken_string(Token t,QByteArray &out)   //解密csv文件
{
    QString path = "/tmp/" + selectFile(t);   //获取解密文件路径
    //QByteArray out;   //函数返回对象
    QFile file(path);
    if(!file.open(QFile::ReadOnly))  //从temp目录下的文件中读取数据
    {
        std::cout<<"文件打开失败"<<std::endl;
        return 0;
    }
    bool isFirstLine = 1;
    while(!file.atEnd())
    {
//        QByteArray line = file.readLine();

//         QVector<char>::iterator i;
//         for (i = line.begin() + 8; i != line.end(); ++i)
//             *i = t.value < 5 ? *i - t.value : *i + t.value;
//         out+=line;
        QByteArray line = file.readLine();
        if(isFirstLine)
        {
            isFirstLine = 0;
            out+=line;
            continue;
        }
         QVector<char>::iterator i;
         for (i = line.begin() + 8; i != line.end(); ++i)       //解密
             *i = t.value < 5 ? *i - t.value : *i + t.value;
         out+=line;
    }
    file.close();

    QByteArray path1 = "/home/wwh/catkin_ws/src/RT_OD_ros/test/" + selectFile(t);
    QFile file1(path1);
    file1.open(QFile::WriteOnly);
    QTextStream wr(&file1);//写入
    wr << out;
    file1.close();
    return 1;
}

bool DecodeToken_byte(Token t,QVector<byte> &data)         //解密lms和pcap文件 可能需要大小端转换
{
    if(t.typeOfToken == 'R')    //如果文件是单线激光雷达
    {
        QString path = "/tmp/" + selectFile(t);
        QFile file(path);
        if(!file.open(QFile::ReadOnly))
        {
            std::cout<<"文件打开失败"<<std::endl;
            return 0;
        }
        QDataStream in(&file);
        float AngleRange;
        //in>>AngleRange;
        in.readRawData((char*)&AngleRange,4);   //获取角度范围
        float AngularResolution;
        //in>>AngularResolution;
        in.readRawData((char*)&AngularResolution,4);    //获取角分辨率
        in.device()->seek(0);   //数据流回到开头重新获取
        char data1[16];
        in.readRawData(data1,16);   //读取前面的16个字节
        int n = (int(AngleRange/AngularResolution) + 1) * 2;
        char Laser[n];  //获取激光束长度
        in.readRawData(Laser,n);
        for(int i = 0;i<n;i++)      //解密
           Laser[i] = (Laser[i] + (-1) * t.value)%256;
        //byte data[16 + n];           //得到最终数组
        //QVector<byte> data;
        for(int i = 0;i<16;i++)
           // data[i] = data1[i];
            data.push_back(data1[i]);
        for(int i = 0;i<n;i++)
            //data[i+16] = Laser[i];
            data.push_back(Laser[i]);

        file.close();
          return 1;
    }

    else{             //如果文件是多线激光雷达

        QString path = "/tmp/" + selectFile(t);
        QFile file(path);
        //QFileInfo info(path);
        //int n = info.size();  //获取文件的总字节长度，得到转换后的数组长度
        //byte data[n];  //存储解密后的数据，函数范围数组
        //static int m = 0;
        if(!file.open(QFile::ReadOnly))
        {
            std::cout<<"文件打开失败"<<std::endl;
            return 0;
        }
        //QVector<byte> data;     //函数最终要返回的vector
        QDataStream in(&file);
        //in.device()->pos();
        while(!in.atEnd())  //如果数据流没有读到末尾，就一直循环
        {
            int dataLength;
            in.device()->seek(36);  //数据流跳转到数据实际长度
            //in >> dataLength;  //将数据头中的数据实际长度赋给dataLength
            in.readRawData((char*)&dataLength,4);
            in.device()->seek(0);  //回到数据流的开头

            char fileHeader[24];
            in.readRawData(fileHeader,24);  // 从数据流中读取文件头

            char dataHeader[16];
            in.readRawData(dataHeader,16);  //从数据流中读取数据头

            char dataPack[dataLength];
            in.readRawData(dataPack,dataLength); //从数据流中读取数据包

            for(int i = 0;i<dataLength;i++)      //解密
               dataPack[i] = (dataPack[i] + (-1) * t.value)%256;

            for(int i = 0;i<24;i++)
                //data[m++] = fileHeader[i];
                data.push_back(fileHeader[i]);
            for(int i = 0;i<16;i++)
                //data[m++] = dataHeader[i];
                data.push_back(dataHeader[i]);
            for(int i = 0;i<dataLength;i++)
                //data[m++] = dataPack[dataLength];
                data.push_back(dataPack[dataLength]);
        }
        return 1;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
  //  Widget w;
   // w.show();

//----------------------------------接口初始化---------------
    TCPServer server1(7001,3);       //用于接受指令
    server1.start();

    TCPServer server2(7002,12);        //用于接受令牌
    server2.start();

//-----------------------------等待准备考试指令-----------------------
    std::cout << "等待准备考试指令..." << std::endl;
    QByteArray signal;
    while (!server1.readFixedData(signal));
    if ((signal.at(0) == '\x02') && (signal.at(1) == '\x30') && (signal.at(2) == '\x03'))
            std::cout << "收到准备考试指令" << std::endl;
    else {
            std::cout << "准备考试指令错误，退出程序 " << std::endl;
            return 0;
    }
//-----------------------------发送准备就绪指令------------------------
    std::cout << "发送准备就绪指令" << std::endl;
    char *data;
    data = new char[3];
    data[0] = '\x02'; data[1] = '\x32'; data[2] = '\x03';
    QHostAddress address = QHostAddress::LocalHost; //address 也就是主机号到时候得根据裁判方的IP地址进行调整
    TCPClient client1(address, 7000);			//启动7000端口发送指令
    client1.connectServer();
    client1.writeData(data,3);

//--------------------------等待开始测试指令---------------------------
    while (!server1.readFixedData(signal));
    if ((signal.at(0) == '\x02') && (signal.at(1) == '\x31') && (signal.at(2) == '\x03'))
                std::cout << "收到开始测试指令" << std::endl;
    else {
            std::cout << "开始测试指令错误，退出程序" << std::endl;
            return 0;
    }

    client1.closeConnect(); //接受到开始测试指令后可以关闭发送指令客户端，等待答案传输完成后再开启
    TCPClient client2(address, 7003);			//启动7003端口发送答案
    client2.connectServer();

//-----------------------开始接受令牌------------------------------
    std::cout<<"令牌接收中..."<<std::endl;


    //...................开始读取令牌........................

        QVector<Token> Processing;  //创建令牌向量
        QByteArray oneDatagram;
        Token recToken;        //实例化令牌数据结构
        int flag = 0;    //flag用于标记令牌是否发送完毕



    while(1){
        while (server2.readFixedData(oneDatagram))
        {
//            std::cout << "Receving Token..." <<std::endl; //测试用
//            std::cout<<"length of oneDatagram:"<<oneDatagram.length()<<std::endl;
//            std::cout<<"data of oneDatagram:"<<oneDatagram.data()<<std::endl;
            if ((oneDatagram.at(0) == '\x02') && (oneDatagram.at(11) == '\x03'))
                        std::cout << "收到一个令牌" << std::endl;
            else {
                    std::cout << "令牌格式错误" << std::endl;
                    return 0;
            }
            char tmpT[] = { oneDatagram.at(8),oneDatagram.at(7),oneDatagram.at(6),oneDatagram.at(5),oneDatagram.at(4),oneDatagram.at(3),oneDatagram.at(2),oneDatagram.at(1) }; //时间戳数据进行大小端转化
            memcpy(&(recToken.timeStamp), tmpT, 8);        //校验之后对接收到的数据进行大小端转换。发送过来的数据是大端模式，在这里转换成小端模式。根据自己的编译器情况决定是否进行大小端转换
            recToken.typeOfToken = oneDatagram.at(9);   //获取令牌类型
            recToken.value = int(oneDatagram.at(10));   //获取令牌的值
            Processing.push_back(recToken);     //加入待处理Token序列
//            std::cout<<"length of Processing:"<<Processing.length()<<std::endl;
}

        //------------------------开始传输答案----------------------------

        while(!Processing.isEmpty())
        {
                std::cout<<"解密中(移动文件)..."<<std::endl;
            recToken = Processing.front();
            Processing.pop_front();

            QVector <byte> decryptBytes;     //(应该也可以使用QByteArray类型,后期根据实际情况调整)多线激光雷达和单线激光雷达解密数据，不知道内存大小，使用vector类型

            QByteArray csv;     //其他文件解密数据

            if(recToken.typeOfToken == 'L' || recToken.typeOfToken == 'R')   //如果是多线激光雷达或者单线激光雷达
                 DecodeToken_byte(recToken,decryptBytes);   //对激光雷达文件进行解码得到解码数据
            else DecodeToken_string(recToken,csv);   //对csv文件进行解码得到解码数据

//            //....................编码答案数据................
//            //这里根据解码得到的数据进行作答
            std::cout<<"解答中..."<<std::endl;


        }


        //----------------------判定是否接受令牌发送完毕指令--------------------
        if(server2.isEmpty() && server1.readFixedData(signal)){
        if ((signal.at(0) == '\x02') && (signal.at(1) == '\x33') && (signal.at(2) == '\x03'))
        {
                flag = 1;
                std::cout << "收到令牌发送完毕指令" << std::endl;
        }
        else {
                std::cout << "令牌发送完毕指令错误，退出程序" << std::endl;
                return 0;
        }
        std::cout<<"令牌接收完毕"<<std::endl;
        if(flag) break;
    }
}


//----------------------发送答案传输完成指令-------------------------
std::cout<<"发送答案传输完成指令.."<<std::endl;
data[0] = '\x02'; data[1] = '\x35'; data[2] = '\x03';
client1.connectServer(); //启动发送指令客户端
client1.writeData(data,3);
client1.closeConnect();


    return a.exec();
}
