# 基于C++98/C++11的线程池

----------------
C++98 线程池
利用posix的pthread接口实现多线程操作
面向对象编程

主要代码说明：
ThreadPool类，不允许复制操作
public:
ThreadPool(int);//构造函数，开启线程池数目，启动线程池
~ThreadPool();//析构函数，处理资源
公开接口：
int addTask(Task *task);//添加任务
void stop();//停止线程池
 int size();//任务数目
Task* take();//获取任务队列的第一个任务

----------------
C++11 线程池
C++11全新的thread线程库实现多线程操作
基于对象编程（std::function/std::bind）




