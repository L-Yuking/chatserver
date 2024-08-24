/*muduo网络库给用户提供了两个主要的类
TcpServer:用于编写服务器程序的类
TcpClient:用于编写客户端程序的类
将epoll和线程池封装起来，好处是能够把网络的I/O代码与业务代码区分开
*/

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional> //绑定器
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*基于muduo网络库开发服务器程序
1.组合TcpServer对象
2.创建EventLoop事件循环对象的指针
3.明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
4.在当前服务器类的构造函数中，注册处理连接和读写事件的回调函数
5.设置合适的服务端线程数量，muduo会自动划分I/O线程和worker线程
*/
class ChatServer
{
public:
    ChatServer(EventLoop *loop,                           // 事件循环 #3
               const InetAddress &listenAddr,             // IP地址 端口
               const string &nameArg)                     // 服务器的名字，给线程绑定个名字
        : _server(loop, listenAddr, nameArg), _loop(loop) // loop 把事件循环保存起来
    {
        // #4
        // 用户创建和断开连接的回调,网络库帮我监听，只知道创建和断开连接后在回调函数里开发什么业务.当底层监听到有用户连接和创建，就调用这个函数
        _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1)); //&地址

        // 用户读写事件回调
        _server.setMessageCallback(bind(&ChatServer::onMessage,this,_1,_2,_3));
        //#5 设置服务器的线程数量 一个io线程，三个worker线程
        _server.setThreadNum(4);
    }
    //开启事件循环
    void start(){
        _server.start();
    }

private:
    // 专门处理用户连接的创建和断开  epoll ->listenfd->accept(有新用户连接了)
    void onConnection(const TcpConnectionPtr &conn)
    {
        if(conn->connected()){//连接成功
            cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<"state:online"<<endl;
        }else{
            cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<"state:offline"<<endl;
            conn->shutdown();//close(fd),释放socket
            //_loop->quit();//服务器结束，连接断开
        }
    }

    //处理用户读写事件
    void onMessage(const TcpConnectionPtr &conn,//表示连接
                   Buffer *buffer,//缓冲区
                   Timestamp time)//接收到数据的时间信息
    {
        string buf=buffer->retrieveAllAsString();//接收的数据放到字符串中
        cout<<"收到的数据："<<buf<<" time:"<<time.toString()<<endl;//时间信息转换成字符串

        //返回
        conn->send(buf);

    }

    TcpServer _server; // #1
    EventLoop *_loop;  // #2  epoll
};


int main(){
    EventLoop loop;                      // 事件循环 #3
    InetAddress addr("192.168.66.200",7008);        // IP地址 端口
    ChatServer server(&loop,addr,"CharServer");

    server.start();//启动服务，listenfd epoll_ctl添加到epoll上
    loop.loop();//epoll_wait 以阻塞方式等待新用户连接和读写时间
    return 0;
}
