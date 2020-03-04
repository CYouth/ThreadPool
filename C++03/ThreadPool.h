#ifndef THREADPOOL
#define THREADPOOL

#include<pthread.h>
#include<queue>
#include"Task.h"
#include<iostream>

using namespace std;

//RAII��ʽ����mutex
class LockGuard
{
public:
	LockGuard(pthread_mutex_t& mutex):mutex_(mutex){
		pthread_mutex_lock(&mutex_);//����
	}
	~LockGuard() {
		pthread_mutex_unlock(&mutex_);//����
	}

private:
	pthread_mutex_t& mutex_;
};

//�̳߳���
class Threadpool
{
public:
	Threadpool(int threadsNum = 10);//���캯��
	~Threadpool();//��������
	int size();//�����������
	int addTask(Task *task);//������񣬷����������
	void stop();//�����̳߳أ����������߳�

private:
	int threadsNum_;//�߳���
	pthread_t* threads_;//�߳�����
	queue<Task*> taskQueue_;//�������
	pthread_mutex_t mutex_;//������
	pthread_cond_t condition_;//��������
	bool isRunning;
	
	void createThreads();//�����߳�
	static void* threadFunc(void* args);//�߳�ִ�к�����һ��Ҫ��void*(void*)��ʽ��
	Task* take();//���������ȡ������,blocking

	Threadpool(const Threadpool&);//�������캯��
	Threadpool& operator=(const Threadpool&);

};

#endif // !THREADPOOL