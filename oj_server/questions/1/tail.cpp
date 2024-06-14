//测试
#ifndef COMPILER_ONLINE

#include"header.cpp"

#endif // !COMPILER_ONLINE

void Test1(){
    //定义临时对象 完成方法调用
    bool ret = Solution().isPalindrome(121);

    if(ret){
        std::cout<<"通过测试用例1,121通过"<<std::endl;
    }else{
        std::cout<<"测试用例未通过121"<<std::endl;
    }
}

void Test2(){
    //定义临时对象 完成方法调用
    bool ret = Solution().isPalindrome(-10);

    if(ret){
        std::cout<<"通过测试用例1,-10通过"<<std::endl;
    }else{
        std::cout<<"测试用例未通过-10"<<std::endl;
    }
}

int main(){
    Test1();
    Test2();

    return 0;
}