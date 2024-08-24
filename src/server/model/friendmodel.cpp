#include "friendmodel.hpp"
#include "db.hpp"
// 添加好友
void FriendModel::insertFriend(int userid, int friendid)
{
    // 1组装sql语句
    char sql[1024] = {0};
    // sprintf字符串连接
    sprintf(sql, "insert into friend values (%d,%d);", userid,friendid);

    // 2定义mysql对象，对mysql数据操作前，要先进行连接，conn
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 查找好友关系，返回用户的好友列表  做两个表的连接查询
vector<User> FriendModel::query(int id)
{
    // 1组装sql语句
    char sql[1024] = {0};
    // sprintf字符串连接
    sprintf(sql, "select a.id,a.name,a.state from user a \
    inner join friend b on b.friendid = a.id where b.userid=%d", id);

    vector<User> vec;
    // 2定义mysql对象，对mysql数据操作前，要先进行连接，conn
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *result = mysql.query(sql); // 从res中读取数据
        if (result != nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(result))!=nullptr){// 用这个函数从result中获取多行,一行一行的拿给row
                //把userid用户的所有离线消息放入vec中
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            } 

            mysql_free_result(result);//释放
            return vec;
        }
    }
    return vec;
}