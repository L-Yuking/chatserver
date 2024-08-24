# chatserver
基于muduo实现，可以工作在nginx tcp负载均衡环境中的集群聊天服务器和客户端，redis，mysql

# 项目编译
## 1. 下载后编译
cd build
rm -rf * #这是删除之前编译擅长的各种文件，不需要
cmake ..
make

也可以创建编译脚本：mk.sh，如下：
```
#!/bin/sh

rm -rf build/* bin/*
cd build
cmake ..
make
```
最后执行./mk.sh

## 2.运行
需要先启动redis和nginx服务器（要下载安装），可以在csdn上搜索安装教程
```
mysql -u root -p #启动mysql

redis-server /myredis/redis.conf

cd /usr/local/nginx/sbin/
./nginx

# 查看redis和nginx、mysql是否已启动
netstat -antp
```
make编译完成会在/bin目录下生成两个可执行文件ChatClient和ChatServer。分别运行
```
./ChatServer 127.0.0.1 6000 或者./ChatServer 127.0.0.1 6002

#另一个终端
./ChatClient 127.0.0.1 8000 #nginx 负载均衡，客户端连接时用端口号8000
```

netstat -antp
