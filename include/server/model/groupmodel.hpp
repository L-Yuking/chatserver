#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include <string>
#include <vector>
class GroupModel
{
public:
    //创建群组
    bool creatGroup(Group &group);

    //加入群组
    void joinGroup(int userid,int groupid,string role);

    //查询用户所在的群组信息
    vector<Group> queryGroups(int userid);

    //根据指定的群组id查询群组用户的id列表。主要用于群聊，查找除了userid的用户id，利用_userConnMap进行转发
    vector<int> queryGroupUsers(int groupid,int userid);
};


#endif