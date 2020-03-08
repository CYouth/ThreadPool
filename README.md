# 基于C++的线程池

----------------  
基于C++03的线程池  
利用pthread线程库实现多线程操作  

主要代码说明：  
ThreadPool类，线程池管理类  
public:  
ThreadPool(int);//构造函数，开启线程池数目，启动线程池  
~ThreadPool();//析构函数，处理资源  
公开接口：  
int addTask(Task * task);//添加任务  
void stop();//停止线程池  
int size();//任务数目  
Task * take();//获取任务队列的第一个任务  

----------------  
基于C++11的线程池  
C++11 thread线程库实现多线程操作  



