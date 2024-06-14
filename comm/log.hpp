//该模块主要是实现错误日志功能
#pragma once 
#include "util.hpp"
namespace ns_log{
    using namespace ns_util;
    
    //定义日志等级
    enum Level{
        INFO,   //正常提示
        BEDUG,  //用于程序员调试
        WARING, //警告 程序可以运行但是有错误     
        ERROR,  //出错 用户无法继续
        FATAL,  //致命错误 服务器无法继续
    };
    //函数功能：输出文件错误
    //函数返回值：ostream 将文件重定向到cout中
    //函数参数：1 level 日志错误等级 2 日志名称 3 日志错误行
    static std::ostream& log(std::string level, std::string file_name, int line){
        std::string message;

        //添加日志等级
        message += "[";
        message += level;
        message += "]";

        //添加对应文件的文件名和对应的行号
        message += "[";
        message += "file_name";
        message += ":";
        message += std::to_string(line);
        message += "]";

        //添加时间戳
        message += "[";
        message += "time:";
        message += TimeUtile::GetTimeStamp();
        message += "]";

        std::cout << message;

        return std::cout;
    }

    //简化函数调用
    //只需要传入对应的函数等级 后续会自动的补齐对应文件和行号
    
    #define LOG(level) log(#level, __FILE__, __LINE__)
}
