cmake_minimum_required(VERSION 3.0)
project(testMuduo)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
#配置编译选项 -o -g
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)  # 可调试可执行文件


set(SRC_LIST ./testMuduo.cpp)

add_executable(testMuduo ${SRC_LIST})

target_link_libraries(testMuduo muduo_net muduo_base pthread)