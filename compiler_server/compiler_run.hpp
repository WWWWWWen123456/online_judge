//编译并运行 
//1适配用户请求， 定制通信协议字段
//2正确使用compiler 和run
//3形成唯一文件名

#pragma once 

#include"compiler.hpp"
#include"runner.hpp"
#include<jsoncpp/json/json.h>
#include<signal.h>
#include<unistd.h>
#include"../comm/log.hpp"
#include"../comm/util.hpp"


namespace ns_compiler_and_run{

    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_compiler;
    using namespace ns_runner;

    class CompilerAndRun{
        public:
            static void RemoveTempFile(const std::string &file_name){
                //最多形成的临时文件个数是知道的 具体有多少个并不确定
                //但是具体清除哪些也知道
                std::string _src = PathUtil::Src(file_name);
                //如果存在才移除
                if(FileUtil::IsExist(_src)) unlink(_src.c_str());

                std::string _compiler_error = PathUtil::CompilerError(file_name);
                if(FileUtil::IsExist(_compiler_error)) unlink(_compiler_error.c_str());

                std::string _execute = PathUtil::CompilerError(file_name);
                if(FileUtil::IsExist(_execute)) unlink(_execute.c_str());

                std::string _stdin = PathUtil::CompilerError(file_name);
                if(FileUtil::IsExist(_stdin)) unlink(_stdin.c_str());

                std::string _stdout = PathUtil::CompilerError(file_name);
                if(FileUtil::IsExist(_stdout)) unlink(_stdout.c_str());

                std::string _stderr = PathUtil::CompilerError(file_name);
                if(FileUtil::IsExist(_stderr)) unlink(_stderr.c_str());
            }
            //> 0 ==0 <0
            //区分compiler_and_run 和runner的错误码
            static std::string CodeToDesc(int code, const std::string &file_name){
                //code>0 进程收到了信号导致异常崩溃
                //code<0 服务器内部错误 编译失败 代码为空等问题 整个过程非运行报错
                //code==0 ：整个过程全部完成
                //后续可以根据错误添加 
                //实现错误码转换成错误码描述

                std::string desc;
                switch(code){
                    case 0:
                    desc = "代码运行成功";
                    break;

                    case -1:
                    desc ="用户提交的代码为空";
                    break;
                    
                    case -2:
                    desc = "未知错误";
                    break;

                    case -3:
                    //desc = "代码编译时发生错误";//发生了什么错误？
                    desc = FileUtil::ReadFile(PathUtil::CompilerError(file_name), desc, true);
                    break;
                    
                    case SIGABRT: //6
                    desc = "内存超过范围";
                    break;

                    case SIGXCPU: //24
                    desc = "CPU使用超时";
                    break;

                    case SIGFPE: //8
                    desc = "浮点数溢出";
                    break;

                    default:
                    desc = "debug:"+std::to_string(code);
                    break;

                }
                return desc;
            }
            
            
            //输入：
            //input :用户提交的代码对应的输入， 不做处理
            //code: 用户提交的代码
            //cpu_limit
            //mem_limit

            //输出：
            //status:状态码 0-请求成功 
            //reason ：请求结果 
            //（非必要） 
            //stdout 程序运行完的结果
            //stderr 程序运行完的错误结果

            //out_json{"status":"0", "reason":"", "stdout":"", "stderr":""}
            static void Start(const std::string &in_json, std::string *out_json){
                Json::Value in_value;
                Json::Reader reader;
                reader.parse(in_json, in_value);//最后再处理差错问题

                std::string code = in_value["code"].asString();
                std::string imput = in_value["input"].asString();
                int cpu_limit = in_value["cpu_limit"].asInt();
                int mem_limit = in_value["mem_limit"].asInt();
                
                int status_code = 0;
                Json::Value out_value;
                int run_result = 0;
                std::string file_name;//需要内部形成的唯一文件名
                //差错处理
                if(code.size()==0){
                    //out_value["status"] = 1;//代码为空
                    //out_value["reason"] = "用户提交的代码为空";
                    
                    status_code = -1;
                    goto END;
                    //序列化过程
                    
                }
                //生成文件并且写入 应对多人编译 （需要形成可以区别的文件名）
                //没有目录 没有后缀 需要后续添加
                file_name = FileUtil::UniqueFile();

                if(!FileUtil::WriteFile(PathUtil::Src(file_name), code)){
                    //out_value["status"] = -2;
                    //out_value["reason"] = "提交的代码发生未知错误";
                    
                    status_code = -2;
                    goto END;
                    //序列化过程
                    
                }//形成src临时文件

                if(!Compiler::Compier(file_name)){
                    //编译失败
                    //out_value["status"] = 3;//代码编译的时候发生了错误 用户语法问题
                    //out_value["reason"] = FileUtil::ReadFile(PathUtil::CompilerError(file_name));//编译器的compiler_err文件读取

                    status_code = -3;
                    goto END;

                }
                run_result = Runner::Run(file_name, cpu_limit, mem_limit);
                if(run_result < 0){
                    status_code = -2;//未知错误
                    
                    //out_value["reason"] = "发生了未知错误";
                    
                    //序列化
                    
                }else if(run_result > 0){
                    //程序运行崩溃
                    status_code = run_result;
                    //out_value["status"] = 4;//运行时报错
                    //out_value["reason"] = SignoToDesc(code);//每一种信号代表一种具体的错误原因 设置方法来解析
                    //序列化过程
                }else{
                    //运行成功
                    //运行结果在stdout中
                    status_code = 0;
                }
                END:
                //status_code 
                out_value["status"] = status_code;
                out_value["reason"]= CodeToDesc(status_code, file_name);//原因有很多 封装成函数
                if(status_code == 0){
                    //整个编译过程成功
                    std::string _stdout;
                    FileUtil::ReadFile(PathUtil::Stdout(file_name), _stdout,true);
                    out_value["stdout"] = _stdout;

                    std::string _stderr;
                    FileUtil::ReadFile(PathUtil::Stderror(file_name),_stderr, true);
                    out_value["stdout"] = _stderr;
                }
                //序列化
                Json::StyledWriter writer;
                *out_json = writer.write(out_value);
                
                //清除临时文件
                //RemoveTempFile(file_name);
            }
    };    
}