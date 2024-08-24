#include <iostream>

#include <hiredis/hiredis.h>


int main() {

    // 创建一个Redis连接对象
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            std::cout << "Error: " << c->errstr << std::endl;
            // handle error, close connection, etc.
            redisFree(c);
        } else {
            std::cout << "Can't allocate redis context" << std::endl;
        }
        return 1;
    }

    // 执行一些基本操作，例如设置和获取键值对
    redisReply *reply = (redisReply *)redisCommand(c, "SET key value");
    if (reply == NULL) {
        std::cout << "Error: " << c->errstr << std::endl;
        // handle error, close connection, etc.
        redisFree(c);
        return 1;
    }
    std::cout << "Set result: " << reply->str << std::endl;
    freeReplyObject(reply);

    reply = (redisReply *)redisCommand(c, "GET key");
    if (reply == NULL) {
        std::cout << "Error: " << c->errstr << std::endl;
        // handle error, close connection, etc.
        redisFree(c);
        return 1;
    }
    std::cout << "Get result: " << reply->str << std::endl;
    freeReplyObject(reply);//释放reply指向的结构体防止内存泄漏

    // 释放Redis连接对象
    redisFree(c);

    return 0;
}

