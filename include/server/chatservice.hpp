#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>//一个消息ID映射一个事件处理 
#include <functional>
#include<mutex>


using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "json.hpp"
#include "redis.hpp"
using json = nlohmann::json;

//相当于type define  表示处理消息的事件回调方法类型，事件处理器，派发3个东西 
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

//聊天服务器业务类
class ChatService
{
public:
    //获取单例对象的接口函数
    static ChatService *instance();
    //处理登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //一对一聊天
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //创建群组业务
    void creatGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //加入群组业务
    void joinGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

    //服务器异常，业务重置方法
    void reset();

    //获取消息 对应的处理器
    MsgHandler getHandler(int msgid);

    void handleRedisSubscribeMessage(int userid, string msg);
private:
    ChatService();//单例 构造函数私有化

    //一个消息ID映射一个事件处理
    //存储消息id和其对应的业务处理方法，消息处理器的一个表，写消息id对应的处理操作 
    //事先已经添加好的业务
    unordered_map<int, MsgHandler> _msgHandlerMap;

    //存储在线用户的通信连接 用户id conn
    //要注意线程安全，会有变动，所以要用互斥锁
    //已建立连接的id放到这个表中
    unordered_map<int,TcpConnectionPtr> _userConnMap;

    //定义互斥锁，保证_userConnMap的线程安全
    mutex _connMutex;

    //数据操作类对象,
    //用户操作对象
    UserModel _userModel;

    //离线消息操作对象
    OfflineMsgModel _offlineMsgModel;

    //添加好友
    FriendModel _friendModel;

    //群组业务对象
    GroupModel _groupModel;

    //redis操作对象
    Redis _redis;
};

#endif
