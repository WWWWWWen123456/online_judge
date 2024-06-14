#pragma once 

#include<iostream>
#include<string>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<sys/resource.h>
#include"../comm/log.hpp"
#include"../comm/util.hpp"

namespace ns_runner{
    using namespace ns_util;
    using namespace ns_log;

    class Runner{
        public:
        Runner(){}
        ~Runner(){}
        public:
        //设置进程占用资源大小的接口
        static void SetProcLimit(int _cpu_limit, int _mem_limit){
            struct rlimit cpu_rlimit;
            //设置CPU时长
            cpu_rlimit.rlim_max = RLIM_INFINITY;
            cpu_rlimit.rlim_cur = _cpu_limit;
            setrlimit(RLIMIT_CPU, &cpu_rlimit);
            //设置内存大小
            struct rlimit mem_rlimit;
            mem_rlimit.rlim_max = RLIM_INFINITY;
            mem_rlimit.rlim_cur = _mem_limit;
            setrlimit(RLIMIT_AS, &mem_rlimit);
        }
        //传入文件名自动运行
        //返回值>0 异常 收到对应的信号
        //返回值 == 0 正常运行 形成了一些临时文件 不关心结果
        //返回值<0 内部错误：打开文件 创建子进程等失败
        //cpu_limit 该程序运行的时候 可以使用的最大CPU资源上限
        //mem_limit 该程序运行的时候 可以使用的最大的内存大小（KB）
        static int Run(const std::string &file_name, int cpu_limit, int mem_limit){
            //程序运行的几种结果
            //1 代码跑完 结果正确
            //2 代码跑完 结果不正确
            //3 代码没跑完 异常
            //结果正确与否与测试用例有关 其他没什么关系
            //测试模块只关注代码是否跑完
            //程序启动
            //1 标准输入 不处理 留出接口 上层传入
            //2 标准输出 判断结果是否正确 代表程序完成之后运行结果
            //3 标准错误 程序运行时的错误信息(运行和编译错误)
            //功能1 只关心对应的信息 2 只关注是否跑完
            std::string _execute = PathUtil::Exe(file_name);
            std::string _stdin = PathUtil::Stdin(file_name);
            std::string _stdout = PathUtil::Stdout(file_name);
            std::string _stderror = PathUtil::Stderror(file_name);
            
            umask(0);
            int _stdin_fd = open(_stdin.c_str(), O_CREAT | O_RDONLY, 0644);
            int _stdout_fd = open(_stdout.c_str(), O_CREAT | O_WRONLY, 0644);
            int _stderror_fd = open(_stderror.c_str(), O_CREAT | O_WRONLY, 0644);
            pid_t pid = fork();

            if(_stdin_fd < 0 || _stderror_fd < 0 || _stdout_fd <0){
                //内部错误 不应该暴露给用户 
                LOG(ERROR)<<"运行时打开标准文件失败"<<"\n";
                return -1;//文件打开失败
            }

            //差错处理
            if(pid < 0){
                LOG(ERROR)<<"运行时创建子进程失败"<<"\n";
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderror_fd);
                return -2;//创建子进程失败
            }
            //子进程进程替换进行运行
            else if(pid == 0){
                //重定向
                dup2(_stdin_fd, 0);
                dup2(_stdout_fd, 1);
                dup2(_stderror_fd, 2);

                SetProcLimit(cpu_limit, mem_limit);
                execl(_execute.c_str(), _execute.c_str(), nullptr);
                exit(1);
            }
            //父进程进行处理
            else{
                close(_stdin_fd);
                close(_stderror_fd);
                close(_stdout_fd);

                int status = 0;
                waitpid(pid, &status, 0);
                LOG(INFO)<<"运行完毕， INFO： "<<(status & 0x7F) << "\n";
                return status & 0x7F;//1-31
            }
        }
    };
}
//程序运行异常一定是收到了信号 什么原因？
//程序运行时候形成的临时文件 文件异常毫无意义