#include "groupmodel.hpp"
#include "db.hpp"
#include "group.hpp"

// 创建群组
bool GroupModel::creatGroup(Group &group)
{
    // 1组装sql语句
    char sql[1024] = {0};
    // sprintf字符串连接
    sprintf(sql, "insert into allgroup(groupname,groupdesc) values ('%s','%s');", group.getName().c_str(), group.getDesc().c_str());

    // 2定义mysql对象，对mysql数据操作前，要先进行连接，conn
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 创建成功
            // 还要获取id，mysql.h里面有个方法mysql_insert_id，可以获取主键id。get的是连接
            group.setId(mysql_insert_id(mysql.getConnection()));

            return true;
        }
    }

    return false;
}

// 加入群组
void GroupModel::joinGroup(int userid, int groupid, string role)
{
    // 1组装sql语句
    char sql[1024] = {0};
    // sprintf字符串连接
    sprintf(sql, "insert into groupuser values (%d,%d,'%s');", groupid, userid, role.c_str());

    // 2定义mysql对象，对mysql数据操作前，要先进行连接，conn
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 查询用户所在的群组信息
vector<Group> GroupModel::queryGroups(int userid)
{

    // 功能1.两表进行联合查询,先根据userid在groupuser表中查询该用户所属的所有群组的信息

    // 1组装sql语句
    char sql[1024] = {0};
    // sprintf字符串连接
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from allgroup a \
    inner join groupuser b on b.groupid = a.id where b.userid=%d",
            userid);

    vector<Group> groupVec;
    // 2定义mysql对象，对mysql数据操作前，要先进行连接，conn
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *result = mysql.query(sql); // 从res中读取数据
        if (result != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result)) != nullptr)
            { // 用这个函数从result中获取多行,一行一行的拿给row
                // 把userid用户的所有离线消息放入vec中
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);

                groupVec.push_back(group);
            }

            mysql_free_result(result); // 释放
        }
    }

    // 功能2，根据群组信息，查询属于该群组的所有用户的userid，并和user表进行联合查询，查询到用户的详细信息
    for (Group group : groupVec)
    {
        sprintf(sql, "select a.id,a.name,a.state,b.grouprole from user a \
    inner join groupuser b on b.userid = a.id where b.groupid=%d",
                group.getId());
        MYSQL_RES *result = mysql.query(sql); // 从res中读取数据
        if (result != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result)) != nullptr)
            { // 用这个函数从result中获取多行,一行一行的拿给row
                // 把userid用户的所有离线消息放入vec中
                GroupUser user;//GroupUser继承User类
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);

                group.getGroupUsers().push_back(user);
            }

            mysql_free_result(result); // 释放
        }
    }
    //此时groupVec中存储了userid所在的所有群组group信息，同时group中也存储了该群组下的所有用户信息（getFroupUser()
    return groupVec;
}

// 根据指定的群组id查询群组用户的id列表。主要用于群聊，查找除了userid的用户id，利用_userConnMap进行转发
vector<int> GroupModel::queryGroupUsers(int groupid,int userid)
{
     // 1组装sql语句
    char sql[1024] = {0};
    // sprintf字符串连接
    sprintf(sql, "select userid from groupuser where groupid=%d and userid !=%d",groupid,userid);

    vector<int> idVec;
    // 2定义mysql对象，对mysql数据操作前，要先进行连接，conn
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *result = mysql.query(sql); // 从res中读取数据
        if (result != nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(result))!=nullptr){// 用这个函数从result中获取多行,一行一行的拿给row
               idVec.push_back(atoi(row[0]));
            } 

            mysql_free_result(result);//释放
           
        }
    }
    return idVec;
}