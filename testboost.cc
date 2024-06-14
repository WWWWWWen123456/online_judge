#include<iostream>
#include<boost/algorithm/string.hpp>

int main(){
    std::vector<std::string> tokens;
    const std::string str = "1 判断回文数 简单 1 30000";
    const std::string split = " ";

    boost::split(tokens, str, boost::is_any_of(split), boost::algorithm::token_compress_on);

    for(auto &iter:tokens){
        std::cout<<iter<<std::endl;
    }

    return 0;
}