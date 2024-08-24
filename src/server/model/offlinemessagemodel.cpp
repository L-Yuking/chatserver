#include "offlinemessagemodel.hpp"
#include "db.hpp"
// 存储用户的离线消息
void OfflineMsgModel::insert(int userid, string msg)
{
    // 1组装sql语句
    char sql[1024] = {0};
    // sprintf字符串连接
    sprintf(sql, "insert into offlinemessage values (%d,'%s');", userid, msg.c_str());

    // 2定义mysql对象，对mysql数据操作前，要先进行连接，conn
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 删除用户的离线消息
void OfflineMsgModel::remove(int userid)
{
    // 1组装sql语句
    char sql[1024] = {0};
    // sprintf字符串连接
    sprintf(sql, "delete from offlinemessage where  userid=%d;", userid);

    // 2定义mysql对象，对mysql数据操作前，要先进行连接，conn
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 查询用户的离线消息，离线消息有多个
vector<string> OfflineMsgModel::query(int userid)
{
    // 1组装sql语句
    char sql[1024] = {0};
    // sprintf字符串连接
    sprintf(sql, "select message from offlinemessage where userid=%d ;", userid);

    vector<string> vec;
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
                vec.push_back(row[0]);
            } 

            mysql_free_result(result);//释放
            return vec;
        }
    }
    return vec;
}