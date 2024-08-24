#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include <vector>
#include "user.hpp"
using namespace std;

//维护好友信息的操作接口方法
class FriendModel
{
public:
    //添加好友
    void insertFriend(int userid,int friendid);

    //查找好友关系，返回用户的好友列表  做两个表的连接查询
    vector<User> query(int id);
};




#endif