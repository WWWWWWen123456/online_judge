// //加载所有的题目信息到内存中
// //model：主要用来和数据交互并且对外访问接口的提供
// #pragma once 
// #include<string>
// #include<iostream>
// #include<unordered_map>
// #include<assert.h>
// #include<vector>
// #include<cstdlib>
// #include<fstream>

// #include"../comm/log.hpp"
// #include"../comm/util.hpp"

// namespace ns_model{
//     using namespace std;
//     using namespace ns_log;
//     using namespace ns_util;
    
    

//     struct Question{
//         std::string number;//题目编号 唯一值key
//         std::string title;//标题
//         std::string star;//难度
//         int cpu_limit;//时间要求
//         int mem_limit;//空间要求

//         std::string desc;//题目描述
//         std::string header;//预设给用户的部分代码
//         std::string tail;//测试用例
//     };
//     //文件list的路径
//     const std::string questions_list = "./questions/questions.list";
//     const std::string questions_path = "./questions/";
//     class Model{
//         private:
//         //映射文件信息 
//         //题目和题号的映射关系建立
//         unordered_map<string,Question>questions;
//         public:
//         Model(){
//             assert(LoadQuesionList(questions_list));
//         };//构造函数
//         ~Model(){};//析构函数
//         //加载题目列表
//         bool LoadQuesionList(const std::string &question_list){
//             //加载配置文件
//             ifstream in(question_list);

//             if(!in.is_open()){
//                 LOG(FATAL) <<"加载题库失败 请检查是否存在题库文件"<<"\n";
//                 return false;
//             }

//             //拿到每行数据 
//             //1 判断回文数 简单 1 300
//             std::string line;
//             while(getline(in, line)){
//                 vector<string> tokens;
//                 StringUtil::SplitString(line, &tokens, " ");
//                 if(tokens.size() != 5){
//                     //处理配置文件错误
//                     LOG(WARING)<<"加载题目部分题目失败 请检查文件格式"<<"\n";
//                     continue;
//                 }
//                 //填充question的字段 
//                 Question q;
//                 q.number = tokens[0];
//                 q.title = tokens[1];
//                 q.star = tokens[2];
//                 q.cpu_limit = atoi(tokens[3].c_str());
//                 q.mem_limit = atoi(tokens[4].c_str());
//                 //访问对应题目的位置
//                 std::string question_number_path = questions_path;
//                 question_number_path += q.number; 
//                 question_number_path += "/";
//                 //读取对应的文件信息
//                 FileUtil::ReadFile(question_number_path+"desc.text", q.desc, true);
//                 FileUtil::ReadFile(question_number_path+"header.cpp", q.header, true);
//                 FileUtil::ReadFile(question_number_path+"tail.cpp", q.tail, true);

//                 questions.insert({q.number, q});

//             }
//             LOG(INFO)<<"加载题库成功"<<"\n";
//             in.close();
//         }
            
//         //获取所有题目
//         bool GetAllQuesions(vector<Question>*out){
//             if(questions.size() == 0) {
//                 LOG(ERROR)<<"用户获取题库失败"<<"\n";
//                 return false;
//             }

//             for(const auto &q:questions){
//                 out->push_back(q.second); //first:key second:v题目信息
//             }
//             return true;
//         }
//         //获取单个题目的信息
//         bool GetOneQuestion(const std::string &number, Question* q){
//             const auto & iter = questions.find(number);
//             if(iter == questions.end()){
//                 LOG(ERROR)<<"用户获取题目："<<number<<"失败"<<"\n";
//                 return false; 
//             }

//             (*q) = iter->second;
//             return true;
//         }
//     };
// }