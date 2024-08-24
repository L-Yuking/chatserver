#ifndef USERMODEL_H
#define USERMODEL_H

#include<string>
#include "user.hpp"
using namespace std;

//User表的数据操作类，不是业务，只针对表的数据进行操作
class UserModel
{
public:
    //user表的增加方法
    bool insert(User &user);

    //user表的查询方法,根据用户id
    User select(int id);


    //user的更新用户状态操作
    bool updateState(User &user);

    //重置用户的状态信息
    void resetState();

};



#endif