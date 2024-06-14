// #include"compiler_run.hpp"

// using namespace ns_compiler_and_run;

// //编译服务随时可能被多个人请求 ， 必须保证传递上来的code， 形成原文件名称的时候具有唯一性
// //否则多个用户空间会相互影响
// int main(){
//    //通过http让client给我们上传一个json string 
   
//    //充当客户端请求的json串
//     std::string in_json;
    
//     Json::Value in_value;
//     in_value["code"]=R"(#include<iostream>
//     int main(){
        
//         std::cout<<"你可以看见我了"<<std::endl;
//         //测试cpu_limit
//         while(1);
//         //测试mem_limit
//         //int* p = new int[1024*1024*1024];
//         //测试编译错误
//         //aaaaa
//         return 0;
//     })";
//     in_value["input"]="";
//     in_value["cpu_limit"]=1;
//     in_value["mem_limit"]=10240*3;//30M
    
    
//     Json::FastWriter writer;
//     in_json = writer.write(in_value);
//     std::cout<<in_json<<std::endl;
    
//     std::string out_json;//将来给客户端返回的json串
//     CompilerAndRun::Start(in_json, &out_json);

//     std::cout<<out_json<<std::endl;
//     //return 0;
// }

#include"compiler_run.hpp"
#include"../comm/httplib.h"
#include <regex>
using std::regex;
using namespace ns_compiler_and_run;
using namespace httplib;

void Usage(std::string proc){
    std::cerr << "Usage:" << "\n\t"<<proc<<"port"<<std::endl;
}
//./compiler_server port
int main(int argc, char *argv[]){

    if(argc !=2){
        Usage(argv[0]);
        return 1;
    }
    Server svr;

    svr.Get("/hello",[](const Request &req, Response &resp){
        resp.set_content("hello http,你好httplib!", "text/plain;chartset=utf8");
    });

    svr.Post("/compile_and_run",[](const Request &req, Response &resp){
        //用户请求的服务正文就是我们想要的json string
        std::string in_json = req.body;
        // debug
        std::cout << "recv reqest: " << in_json << std::endl;
        std::string out_json;
        if(!in_json.empty()){
            CompilerAndRun::Start(in_json, &out_json);
            resp.set_content(out_json, "application/json;charset=utf-8");
        } 
    });
    //svr.set_base_dir("./wwwroot");
    svr.listen("0.0.0.0", atoi(argv[1]));

}