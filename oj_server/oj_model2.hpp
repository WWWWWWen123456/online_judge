//加载所有的题目信息到内存中
//model：主要用来和数据交互并且对外访问接口的提供
//基于MYSQL的 

#pragma once 
#include<string>
#include<iostream>
#include<unordered_map>
#include<assert.h>
#include<vector>
#include<cstdlib>
#include<fstream>
#include"include/mysql.h"

#include"../comm/log.hpp"
#include"../comm/util.hpp"

namespace ns_model{
    using namespace std;
    using namespace ns_log;
    using namespace ns_util;
    
    
    const std::string oj_questions = "questions";
    const std::string host = "127.0.0.1";
    const int port = 3306;
    const std::string user = "oj_client";
    const std::string passwd = "Azhengyawen666@";
    const std::string db = "oj";

    struct Question{
        std::string number;//题目编号 唯一值key
        std::string title;//标题
        std::string star;//难度
        std::string desc;//题目描述
        std::string header;//预设给用户的部分代码
        std::string tail;//测试用例
        int cpu_limit;//时间要求
        int mem_limit;//空间要求
    };

    class Model{
        public:
        Model(){}//构造函数
        ~Model(){}//析构函数
        bool QueryMysql(const std::string &sql, vector<Question>*out){
            //创建mysql句柄
            MYSQL * mysql = mysql_init(nullptr);
            //链接数据库
            if(mysql_real_connect(mysql, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0) == nullptr){
                LOG(FATAL) <<"连接数据库失败: "<< mysql_error(mysql) << "\n";
                return false;
            }
            //设置编码格式 否则页面上会有乱码
            mysql_set_character_set(mysql, "utf8");
            LOG(INFO)<<"链接数据库成功"<<"\n";

            //数据访问
            //差错处理
            if(mysql_query(mysql, sql.c_str()) != 0){
                LOG(WARING) <<sql <<"execute error!"<<"\n";
                return false;
            }
            
            //提取分析结果
            MYSQL_RES *res = mysql_store_result(mysql);
            int rows = mysql_num_rows(res);//行数量
            int cols = mysql_num_fields(res);//列数量
            Question q;

            for(int i = 0; i < rows; i++){
                MYSQL_ROW row = mysql_fetch_row(res);
                q.number = row[0];
                q.title = row[1];
                q.star = row[2];
                q.desc = row[3];
                q.header = row[4];
                q.tail = row[5];
                q.cpu_limit = atoi(row[6]);
                q.mem_limit = atoi(row[7]);

                out->push_back(q);
            }
            //释放结果空间 防止内存泄漏
            free(res);
            //关闭mysql链接
            mysql_close(mysql);

            return true;
        }
        //获取所有题目
        bool GetAllQuesions(vector<Question>*out){
            std::string sql = "select * from ";
            sql += oj_questions;
            
            return QueryMysql(sql, out);
        }
        //获取单个题目的信息
        bool GetOneQuestion(const std::string &number, Question* q){
            bool res = false;

            std::string sql = "select * from ";
            sql += oj_questions;
            sql += " where id=";
            sql += number;
            
            vector<Question>result;
           
            if(QueryMysql(sql, &result)){
                if(result.size() == 1){
                    *q = result[0];
                    res = true;
                }
            }
            return res;
        }
    };
}
