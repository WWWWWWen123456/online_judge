//测试资源限制

#include<iostream>
#include<sys/time.h>
#include<sys/resource.h>
#include<unistd.h>
#include<signal.h>

#include<jsoncpp/json/json.h>
int main(){
    //序列化工作
    //将结构化数据转化成为一个字符串 
    //Value是一个json中间类 可以填充kv值
    Json::Value root;
    root["code"] = "mycode";
    root["user"] = "zyw";
    root["age"] = "21";

    Json::StyledWriter writer;
    std::string str = writer.write(root);
    std::cout << str <<std::endl;
}
// void handler(int signo){
//     std::cout<<"signo:"<<signo<<std::endl;
//     exit(1);
// }
// int main(){
//     for(int i = 1; i<=31; i++){
//         signal(i, handler);
//     }
//     //资源不足 导致OS终止进程 是通过信号终止的
//     //限制累计运行时长
//     //没有资源约束 则一直死循环
//     //需要设置累计运行时长 不要死循环
//     // struct rlimit r;
//     // r.rlim_cur = 1; //累计描述1秒
//     // r.rlim_max = RLIM_INFINITY; //无穷的 硬资源限制
//     //setrlimit(RLIMIT_CPU, &r);
//     //while(1);
    
//     //设置空间资源
//     struct rlimit r;
//     r.rlim_cur = 1024*1024*20;//29M
//     r.rlim_max = RLIM_INFINITY;
//     setrlimit(RLIMIT_AS ,&r);

//     int count = 0;
//     while(1){
//         int* p = new int[1024*1024];
//         count++;
//         std::cout<<"size:"<<count<<std::endl;
//         sleep(1);
//     }
//     return 0;
// }