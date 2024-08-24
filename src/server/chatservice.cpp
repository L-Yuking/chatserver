#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h> //muduo的日志
#include <iostream>
#include <vector>
using namespace std;
using namespace muduo;

// 获取单例对象的接口函数
ChatService *ChatService::instance()
{ // 唯一的实例
    static ChatService service;
    return &service;
}

// 构造方法，注册消息以及对应的Handler回调操作
ChatService::ChatService()
{

    // 用户基本业务管理相关事件处理回调注册
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::creatGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({JOIN_GROUP_MSG, std::bind(&ChatService::joinGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&ChatService::loginout, this, _1, _2, _3)});

    // 连接redis服务器
    if (_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}

// 服务器异常，业务重置方法
void ChatService::reset()
{
    // 把online状态的用户，设置成offline
    _userModel.resetState();
}

// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
    // 记录错误日志，msgid没有对应的事件处理回调

    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end()) // 找不到
    {
        // 返回一个默认的处理器，空操作，=按值获取
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp)
        {
            LOG_ERROR << "msgid:" << msgid << " 没有找到相应的事务处理!"; // muduo日志会自动输出endl
        };
    }
    else // 成功的话
    {
        return _msgHandlerMap[msgid]; // 返回这个处理器
    }
}

//{"msgid":1,"name":"lalallalla","pwd":"123","id":23}
// 处理登录业务  id  pwd
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _userModel.select(id);
    if (user.getId() == id && user.getPwd() == pwd)
    {
        if (user.getState() == "online")
        {
            // 该用户已经登陆
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "this account is using!";
            conn->send(response.dump());
        }
        else
        {
            // 登录成功，记录用户连接信息,要考虑线程安全问题
            {
                // 互斥锁作用域
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn}); // 重点在这个语句上，已连接的id和连接插入到这个表中上
            }
            // id用户登陆成功，向redis订阅channel(id)
            _redis.subscribe(id);

            // 登录成功，更新用户状态信息
            user.setState("online");
            _userModel.updateState(user);

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0; // 业务成功
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 查询该用户是否有离线消息，有的话传到json里面
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec; // json可直接跟容器进行序列化或者反序列化

                // 删除该用户的所有离线消息
                _offlineMsgModel.remove(id);
            }

            // 查看好友列表并返回
            vector<User> userVec = _friendModel.query(id);
            if (!userVec.empty())
            {
                vector<string> friendVec;
                for (User &user : userVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    friendVec.push_back(js.dump());
                }
                response["friends"] = friendVec;
            }

            // 查询用户的群组信息
            vector<Group> groupuserVec = _groupModel.queryGroups(id);
            if (!groupuserVec.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupV;
                for (Group &group : groupuserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    vector<string> userV;
                    for (GroupUser &user : group.getGroupUsers())
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }

                response["groups"] = groupV;
            }

            conn->send(response.dump());
        }
    }
    else
    {

        // 该用户不存在或者密码错误，登录失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1; // 业务失败
        response["errmsg"] = "id or password is invalid! ";
        conn->send(response.dump());
    }
}

// 处理注册业务  name  password
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if (state)
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0; // 业务成功
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1; // 业务失败

        conn->send(response.dump());
    }
}

// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"];
    {
        // 互斥锁作用域
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid);

    User user(userid, "", "", "offline");
    _userModel.updateState(user);
}

// 处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        // 互斥锁作用域
        lock_guard<mutex> lock(_connMutex);

        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                // 从map表删除连接用户的信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(user.getId());

    // 更新用户的状态信息.没找到的话就不用给数据库进行请求了
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

// 一对一聊天
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["toid"].get<int>();

    {
        // 互斥锁作用域
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid); // 在存储连接的表中查找toid有没有，即有代表在线
        if (it != _userConnMap.end())
        {
            // 有,即toid在线,转发消息，
            it->second->send(js.dump()); // js转成字符串，主动推送消息给toid用户
            return;
        }
    }
    // 查询toid是否在线,在线说明在其他服务器上在线
    User user = _userModel.select(toid);
    if (user.getState() == "online")
    {
        _redis.publish(toid, js.dump());
        return;
    }

    // 没有，即toid不在线，要存储离线消息
    _offlineMsgModel.insert(toid, js.dump());
}

// 添加好友业务  id   friendid
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 存储好友信息
    _friendModel.insertFriend(userid, friendid);
}

// 创建群组业务
void ChatService::creatGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string groupname = js["groupname"];
    string groupdecs = js["groupdecs"];

    Group group(-1, groupname, groupdecs); // 存储新创建的群组信息

    bool state = _groupModel.creatGroup(group);
    if (state)
    {
        // 存储群组创建人的信息，role=创建者
        _groupModel.joinGroup(userid, group.getId(), "creator");
        json response;
        response["groupid"] = group.getId();
        conn->send(response.dump());
    }
}

// 加入群组业务
void ChatService::joinGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();

    _groupModel.joinGroup(userid, groupid, "normal");
}

// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();

    vector<int> userVec = _groupModel.queryGroupUsers(groupid, userid);
    // 互斥锁作用域
    lock_guard<mutex> lock(_connMutex);

    for (int id : userVec)
    {

        auto it = _userConnMap.find(id); // 有代表在线
        if (it != _userConnMap.end())
        {
            // 有,转发消息，
            it->second->send(js.dump()); // js转成字符串，主动推送消息给在线用户
            return;
        }
        else
        {
            //查询id是否在线
            User user = _userModel.select(id);
            if (user.getState() == "online")
            {
                _redis.publish(id, js.dump());
            }
            else
            {
                // 存储离线群消息
                _offlineMsgModel.insert(id, js.dump());
            }
        }
    }
}

void ChatService::handleRedisSubscribeMessage(int userid, string msg){
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}
