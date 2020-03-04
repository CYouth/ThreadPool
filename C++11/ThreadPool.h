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
	Threadpool(int threadsNum = 10);//���캯��
	~Threadpool();//��������
	int size();//�����������
	int addTask(const taskType& task);//������񣬷����������
	void stop();//�����̳߳أ����������߳�

private:
	int threadsNum_;//�߳���
	pthread_t* threads_;//�߳�����
	queue<taskType> taskQueue_;//�������
	pthread_mutex_t mutex_;//������
	pthread_cond_t condition_;//��������
	bool isRunning;
	
	void createThreads();//�����߳�
	static void* threadFunc(void* args);//�߳�ִ�к�����һ��Ҫ��void*(void*)��ʽ��
	taskType take();//���������ȡ������,blocking

	Threadpool(const Threadpool&);//�������캯��
	Threadpool& operator=(const Threadpool&);

};

#endif // !THREADPOOL