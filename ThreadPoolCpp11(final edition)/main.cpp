#include <iostream>
#include <string>
#include "ThreadPool.h"
using namespace std;


int main()
{
    std::mutex mtx;
    try//异常处理
    {
        zl::ThreadPool tp;
        std::vector<std::future<int>> v;
        std::vector<std::future<void>> v1;

        for (int i = 0; i <= 10; ++i)
        {
            auto ans = tp.add([](int answer) { return answer; }, i);//返回future
            v.push_back(std::move(ans));//要记得move，直接拷贝会跟之前的promise没有联系
        }

        for (int i = 0; i <= 5; ++i)
        {
            //匿名函数，[&mtx]以引用传进去
            auto ans = tp.add([&mtx](const std::string& str1, const std::string& str2)
            {
                std::lock_guard<std::mutex> lg(mtx);//lock
                std::cout << (str1 + str2) << std::endl;
                return;
            }, "hello ", "world");
            v1.push_back(std::move(ans));
        }
        
        //阻塞获取两个列表的结果
        for (size_t i = 0; i < v.size(); ++i)
        {
            std::lock_guard<std::mutex> lg(mtx);
            cout << v[i].get() << endl;
        }

        for (size_t i = 0; i < v1.size(); ++i)
        {
            std::lock_guard<std::mutex> lg(mtx);
            v1[i].get();
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

}