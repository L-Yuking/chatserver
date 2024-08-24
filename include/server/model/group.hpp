#ifndef GROUP_H
#define GROUP_H

#include <string>
#include <vector>
#include "groupuser.hpp"
using namespace std;
class Group
{
public:
    //构造函数
    Group(int id=-1,string name="",string desc="")
    {
        this->id=id;
        this->name=name;
        this->desc=desc;
        
    }

    void setId(int id){this->id=id;};
    void setName(string name){this->name=name;};
    void setDesc(string desc){this->desc=desc;};
    

    int getId(){return this->id;};
    string getName(){return this->name;};
    string getDesc(){return this->desc;};
    vector<GroupUser> &getGroupUsers(){return this->users;};

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;//从数据库查出一个组有哪些成员，成员放入表中



    
};

#endif