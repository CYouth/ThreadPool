#ifndef THREADPOOL
#define THREADPOOL

#include<pthread.h>
#include<queue>
#include<iostream>
#include<functional>

using namespace std;

typedef function<void()> taskType;

class Threadpool
{
public:
	Threadpool(int threadsNum = 10);//构造函数
	~Threadpool();//析构函数
	int size();//返回任务个数
	int addTask(const taskType& task);//添加任务，返回任务个数
	void stop();//结束线程池，销毁所有线程

private:
	int threadsNum_;//线程数
	pthread_t* threads_;//线程数组
	queue<taskType> taskQueue_;//任务队列
	pthread_mutex_t mutex_;//互斥量
	pthread_cond_t condition_;//条件变量
	bool isRunning;
	
	void createThreads();//创建线程
	static void* threadFunc(void* args);//线程执行函数，一定要是void*(void*)形式的
	taskType take();//任务队列中取出任务,blocking

	Threadpool(const Threadpool&);//拷贝构造函数
	Threadpool& operator=(const Threadpool&);

};

#endif // !THREADPOOL