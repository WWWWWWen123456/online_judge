#pragma once
#include "../comm/util.hpp"
#include "../comm/log.hpp"

namespace ns_compiler{
    using namespace ns_util;
    using namespace ns_log;

    class Compiler{
        public:
            Compiler(){}
            ~Compiler(){}
        //函数功能：子进程去实现文件编译 父进程检测子进程状态并且检测编译是否成功
        //函数参数：文件名 对指定的文件进行编译
        //函数返回值：bool型 编译成功则返回true 如果失败则返回false
        static bool Compier(const std::string& file_name){
            pid_t pid = fork();
            //差错处理
            if(pid < 0){
                LOG(ERROR) << "编译时创建子进程失败" << std::endl;
                return false;
            }
            //子进程
            else if(pid == 0){
                //权限掩码设置为0 不影响权限
                umask(0);
                //打开文件
                int errfd = open(PathUtil::CompilerError(file_name).c_str(), O_CREAT | O_WRONLY, 0644);
                if(errfd < 0){
                    LOG(WARING) << "打开编辑错误文件 （stderr） 失败" <<std::endl;
                    exit(1);
                }
                //将标准错误中的信息重定向到errfd中
                dup2(errfd, 2);
                //进程替换
                execlp("g++", "g++", "-o", PathUtil::Exe(file_name).c_str(),
                PathUtil::Src(file_name).c_str(), "-std=c++11", "-D", "COMPILE_ONLINE",nullptr);
                //差错处理
                //如果进程替换成功的话，接下来的接不会执行 会去执行替换后的进程 如果还在执行的话，说明进程替换失败了

                LOG(ERROR) << "编译时进程替换失败， 请检查gcc参数是否正确" <<std::endl;
                exit(2);
            }
            //父进程
            else{
                //进程等待：回收子进程资源
                //nullptr 不关心子进程状态 0 标识阻塞等待
                waitpid(pid, nullptr, 0);
                //检测编译是否成功
                if(FileUtil::IsExist(PathUtil::Exe(file_name))){
                    LOG(INFO) << "编译成功" <<std::endl;
                    return true;
                }
                LOG(WARING) <<"编译失败，请检查代码是否有误" <<std::endl;
                return false;
            }
        }
    };
}
