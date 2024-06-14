#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

class Solution{
    public:
        bool isPalindrome(int x)
        {
            if(x < 0){
            return false;
        }
        
        int n = 0;
        int m = x;
        while(m > 0){
            n = n * 10 +m % 10;
            m/=10;
        }

        return n == x;
        
    }
        }
};
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif


void Test1()
{
    // 通过定义临时对象，来完成方法的调用
    bool ret = Solution().isPalindrome(121);
    if(ret){
        std::cout << "通过用例1, 测试121通过 ... OK!" << std::endl;
    }
    else{
        std::cout << "没有通过用例1, 测试的值是: 121"  << std::endl;
    }
}

void Test2()
{
    // 通过定义临时对象，来完成方法的调用
    bool ret = Solution().isPalindrome(-10);
    if(!ret){
        std::cout << "通过用例2, 测试-10通过 ... OK!" << std::endl;
    }
    else{
        std::cout << "没有通过用例2, 测试的值是: -10"  << std::endl;
    }
}

int main()
{
    Test1();
    Test2();

    return 0;
}