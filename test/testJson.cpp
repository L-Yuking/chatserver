#include <iostream>
#include <boost/bind.hpp>
#include <string>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

string func(){
    json js;
    js["Msg_type"]=2;
    js["From"]="Zhang Shan";
    string sendBuf = js.dump();     //转成字符串通过网络发送
    return sendBuf;//json字符串
}


int main(){
    string jsontext=func();
    json js=json::parse(jsontext);
    cout<< js["Msg_type"]<<endl;
}
 