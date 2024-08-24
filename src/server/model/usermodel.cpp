#include "usermodel.hpp"
#include "db.hpp"

using namespace std;

// user表的增加方法
bool UserModel::insert(User &user)
{
    // 1组装sql语句
    char sql[1024] = {0};
    // sprintf字符串连接
    sprintf(sql, "insert into user(name,password,state) values ('%s','%s','%s');",
            user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    // 2定义mysql对象，对mysql数据操作前，要先进行连接，conn
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 插入成功
            // 还要获取id，mysql.h里面有个方法mysql_insert_id，可以获取主键id。get的是连接
            user.setId(mysql_insert_id(mysql.getConnection()));

            return true;
        }
    }

    return false;
}

// user表的查询方法
User UserModel::select(int id)
{
    // 1组装sql语句
    char sql[1024] = {0};
    // sprintf字符串连接
    sprintf(sql, "select * from user where id='%d' ;", id);

    // 2定义mysql对象，对mysql数据操作前，要先进行连接，conn
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *result = mysql.query(sql); // 从res中读取数据
        if (result != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(result); // 用这个函数从result中获取一行
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(result); // 释放
                return user;
            }
        }
    }
}

// user的更新操作
bool UserModel::updateState(User &user)
{
    // 1组装sql语句
    char sql[1024] = {0};
    // sprintf字符串连接
    sprintf(sql, "update user set state='%s' where id=%d;", user.getState().c_str(), user.getId());

    // 2定义mysql对象，对mysql数据操作前，要先进行连接，conn
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 成功
            return true;
        }
    }

    return false;
}

// 重置用户的状态信息
void UserModel::resetState()
{
    // 1组装sql语句
    char sql[1024] = "update user set state='offline' where state='online';";
   

    // 2定义mysql对象，对mysql数据操作前，要先进行连接，conn
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
       
    }

    

}