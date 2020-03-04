#include"ThreadPool.h"

Threadpool::Threadpool(int threadNum)
{
	threadsNum_ = threadNum;
	isRunning = true;
	//��ʼ��
	pthread_mutex_init(&mutex_, NULL);
	pthread_cond_init(&condition_, NULL);
	createThreads();
}

Threadpool::~Threadpool()
{
	stop();
}

void Threadpool::createThreads()
{
	threads_ = new pthread_t[threadsNum_];
	for (int i = 0; i < threadsNum_; i++) {
		cout << "create thread " << i << " th" << endl;
		pthread_create(&threads_[i], NULL, threadFunc, this);
	}
}

void * Threadpool::threadFunc(void * args)
{
	pthread_t tid = pthread_self();
	cout << "thread id is " << tid << " begin" << endl;
	Threadpool* ptr = static_cast<Threadpool*>(args);
	while (ptr->isRunning)
	{
		taskType task = ptr->take();//blocking
		//��ʱ���ص�task�������̳߳�stop���µ�
		if (task) {
			//cout << "task address is" << task << endl;
			task();
		}
	}
	cout << "thread id is " << tid << " end" << endl;
	return NULL;
}

taskType Threadpool::take()
{
	pthread_mutex_lock(&mutex_);//����

	//��ֹ��ٻ���
	while (taskQueue_.empty()&&isRunning)
	{
		pthread_cond_wait(&condition_, &mutex_);//�ȴ��ź�
	}
	
	taskType result = nullptr;//����Ϊʲô��Ϊnullptr
	if (isRunning) {
		result = taskQueue_.front();
		taskQueue_.pop();
	}

	pthread_mutex_unlock(&mutex_);//����
	return result;
}

int Threadpool::addTask(const taskType& task)
{
	pthread_mutex_lock(&mutex_);//����
	taskQueue_.push(task);
	int size = taskQueue_.size();
	pthread_mutex_unlock(&mutex_);//����
	pthread_cond_signal(&condition_);//signal����������һ���߳�
	return size;
}

void Threadpool::stop()
{
	//��ֹ�ظ�stop
	if (!isRunning) {
		return;
	}
	cout << "stop the threadpool!!" << endl;
	isRunning = false;
	//���������ڵȴ����߳�
	pthread_cond_broadcast(&condition_);//�㲥

	for (int i = 0; i < threadsNum_; i++)
	{
		pthread_join(threads_[i], NULL);//join�����߳�
	}
	delete[] threads_;
	threads_ = NULL;//��ֹ����ָ��

	pthread_mutex_destroy(&mutex_);
	pthread_cond_destroy(&condition_);
}

int Threadpool::size()
{
	pthread_mutex_lock(&mutex_);//����
	int size = taskQueue_.size();
	pthread_mutex_unlock(&mutex_);//����
	return size;
}
