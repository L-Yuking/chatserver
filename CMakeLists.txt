cmake_minimum_required(VERSION 3.0)#要求cmake最低的版本号
project(chat)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
#set(CMAKE_C_COMPILER "/usr/local/bin/gcc") 
#set(CMAKE_CXX_COMPILER "/usr/local/bin/g++")
#set(CMAKE_CXX_STANDARD 11)

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)  # 可调试可执行文件
 
# 设置可执行文件存储的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
 
#配置头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
 


# 加载子目录
add_subdirectory(src)
 


 