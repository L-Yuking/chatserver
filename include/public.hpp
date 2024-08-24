#ifndef PUBLIC_H
#define PUBLIC_H

/*
是属于server和client的公共文件
*/
enum EnMsgType
{
    LOGIN_MSG = 1,//登录消息，要跟处理登录业务的函数绑定起来
    LOGIN_MSG_ACK ,//登录响应消息
    REG_MSG, //注册消息
    REG_MSG_ACK,//注册响应消息
    ONE_CHAT_MSG, //一对一聊天消息
    ADD_FRIEND_MSG,//添加好友消息

    CREATE_GROUP_MSG,//创建群组
    JOIN_GROUP_MSG,//加入群组
    GROUP_CHAT_MSG,//群组聊天

    LOGINOUT_MSG,//注销

};

#endif
