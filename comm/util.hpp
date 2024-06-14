//该模块主要是用来实现通用工具类
//包括1 路径类 2 时间类 3 文件操作类 4 字符串操作类
#pragma once 
#include<string>
#include<sys/time.h>
#include<iostream>
#include<unistd.h>
#include<sys/stat.h>
#include<atomic>
#include<fstream>
#include<vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include<boost/algorithm/string.hpp>

namespace ns_util{
    //定义一个临时文件的路径 
    const static std::string tmp_path ="../compiler_server/temp/";

    //路径类
    //类的主要功能：为文件添加指定的后缀
    //.cpp .exe  .stderr .stdin .stdout .compiler_err
    //错误分为两个类型 主要是为了区分错误类型
    class PathUtil{
        public:
            PathUtil(){}
            ~PathUtil(){}
        //函数返回值：字符串->返回添加了后缀之后的字符串
        //函数参数：1 指定文件的名称 2 对应的文件的后缀名
        //函数功能：对指定的文件添加指定的后缀并且返回-》主要是后续的复用
        static std::string AddSuffix(std::string file_name, std::string suffix){
            std::string res = tmp_path;
            
            res += file_name;
            res += suffix;

            return res;
        }
        //添加.cpp后缀（源文件）
        static std::string Src(std::string file_name){
           return AddSuffix(file_name, ".cpp");
        }
        //添加 .exe后缀（可执行文件）
        static std::string Exe(std::string file_name){
            return AddSuffix(file_name, ".exe");
        }
        //添加 .stdin后缀（运行结果输入文件）
        static std::string Stdin(std::string file_name){
            return AddSuffix(file_name, ".stdin");
        }
        //添加 .stdout后缀 （运行结果输出文件）
        static std::string Stdout(std::string file_name){
            return AddSuffix(file_name, ".stdout");
        }
        //添加 .stderror后缀 （运行结果错误文件）
        static std::string Stderror(std::string file_name){
            return AddSuffix(file_name, ".stderror");
        }
        //添加 .compiler_err后缀 （编译错误文件）
        static std::string CompilerError(std::string file_name){
            return AddSuffix(file_name, ".compiler_err");
        }

    };
    
    //时间类
    //类的主要功能：1 获取时间戳 2获取毫秒级别时间戳
    class TimeUtile{
        public:
            TimeUtile(){};
            ~TimeUtile(){};

            //获取时间戳
            static std::string GetTimeStamp(){
                struct timeval tv;//定义tv结构体用于接收获取到的时间戳信息
                //差错处理
                if(gettimeofday(&tv, nullptr) == -1){  
                    std::cout << "gettimeofday error" << std::endl;
                    exit(-1);
                }

                return std::to_string(tv.tv_sec);
            }

            //获取毫秒级别的时间戳
            static std::string GetTimsMS(){
                struct timeval tv;

                if(gettimeofday(&tv, nullptr) == -1){
                    std::cout << "gettimeofday error" <<std::endl;
                }
                // 1秒 = 1000毫秒 = 1000000微秒
                return std::to_string(tv.tv_sec*1000 + tv.tv_usec/1000);
            }
    };

    //文件操作类
    //主要功能：1 判断文件的存在性 2 形成独特的文件名 3文件的额一些基本操作（读取 写入 删除）
    class FileUtil{
        public:
            FileUtil(){};
            ~FileUtil(){};

        //函数功能：用来判断文件是否存在
        //参数：对应的文件名
        //返回值 bool类型 true代表存在 false代表不存在
        static bool IsExist(std::string path_name){
            //定义st结构体
            //用来存储对应文件的相关信息
            struct stat st;
            //stat函数
            //功能：获取对应的文件信息 
            //返回值 1. 0:获取成功 2. -1:获取失败
            //参数： 1 文件名（获取哪一个文件） 2 st结构体（输出型参数 获取的结果存储在哪里）
            if(stat(path_name.c_str(), &st) == -1) return false;

            return true;
        }

        //函数功能：形成独一的文件名
        //实现逻辑：1 或许毫秒级的时间戳 2加上原子性的递增数
        static std::string UniqueFile(){
            
            //保证多线程的安全性 ++可能会导致竞争
            static std::atomic<int> uuid(0);
            uuid++;

            std::string res = TimeUtile::GetTimsMS();
            std::string uniq_id = std::to_string(uuid);

            return res + "_" + uniq_id;
        }
        
        //函数功能：写文件。将code写入file_name中
        //函数返回值：bool 如果文件打开失败就返回false 如果打开成功的话 就返回true
        //函数参数： 1 写到哪一个文件里面 2 写什么
        static bool WriteFile(const std::string& file_name, const std::string &code){
            std::ofstream outfile(file_name);
            //打开文件
            //差错处理
            if(!outfile.is_open()){
                return false;
            }

            //操作文件
            outfile.write(code.c_str(), code.size());

            //关闭文件
            outfile.close();

            return true;
        }

        //函数功能：读文件。将src的内容读取到tar中 keep用于对\n进行处理
        //函数返回值：bool 如果文件打开失败就返回false 如果打开成功的话 就返回true
        //函数参数：1 src从哪一个文件读取 2tar读取到哪一个文件中 3 keep是否对\n进行处理。默认情况下不处理，即保留
        static bool ReadFile(const std::string& src_file, std::string& target, bool keep = false){
            //清空target 
            //目的：避免上一次操作对target内容的影响
            target.clear();
            //打开文件
            std::ifstream infile(src_file);
            if(!infile.is_open()) return false;
            //读取文件(逐行读取)
            std::string line;
            while(getline(infile, line)){
                target += line;
                //对\n进行处理
                target += (keep == true ? "\n" : "");
            }
            //关闭文件
            infile.close();

            return true;


        }

        //函数功能：删除文件有关的所有派生文件
        //函数参数：对应的函数名
        static void RemoveTempFile(std::string file_name){
            //获取该文件可能存在的派生文件名
            std::string _src = PathUtil::Src(file_name);
            std::string _exe = PathUtil::Exe(file_name);
            std::string _compile_err = PathUtil::CompilerError(file_name);
            std::string _stdin = PathUtil::Stdin(file_name);
            std::string _stdout = PathUtil::Stdout(file_name);
            std::string _stderr = PathUtil::Stderror(file_name);

            //如果存在就删除
            if(IsExist(_src)) unlink(_src.c_str());
            if(IsExist(_exe)) unlink(_exe.c_str());
            if(IsExist(_compile_err)) unlink(_compile_err.c_str());
            if(IsExist(_stdin)) unlink(_stdin.c_str());
            if(IsExist(_stdout)) unlink(_stdout.c_str());
            if(IsExist(_stderr)) unlink(_stderr.c_str());
        }
    };

    //字符串操作类
    class StringUtil
    {
    public:
        /*************************************
         * str: 输入型，目标要切分的字符串
         * target: 输出型，保存切分完毕的结果
         * sep: 指定的分割符
         * **********************************/
        static void SplitString(const std::string &str, std::vector<std::string> *target, const std::string &sep)
        {
            //boost split
            boost::split((*target), str, boost::is_any_of(sep), boost::algorithm::token_compress_on);
        }
    };
}
