#include"ThreadPool.h"

Threadpool::Threadpool(int threadNum)
{
	threadsNum_ = threadNum;
	isRunning = true;
	//初始化
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
		//此时返回的task可能是线程池stop导致的
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
	pthread_mutex_lock(&mutex_);//加锁

	//防止虚假唤醒
	while (taskQueue_.empty()&&isRunning)
	{
		pthread_cond_wait(&condition_, &mutex_);//等待信号
	}
	
	taskType result = nullptr;//这里为什么能为nullptr
	if (isRunning) {
		result = taskQueue_.front();
		taskQueue_.pop();
	}

	pthread_mutex_unlock(&mutex_);//解锁
	return result;
}

int Threadpool::addTask(const taskType& task)
{
	pthread_mutex_lock(&mutex_);//加锁
	taskQueue_.push(task);
	int size = taskQueue_.size();
	pthread_mutex_unlock(&mutex_);//解锁
	pthread_cond_signal(&condition_);//signal发出，激活一个线程
	return size;
}

void Threadpool::stop()
{
	//防止重复stop
	if (!isRunning) {
		return;
	}
	cout << "stop the threadpool!!" << endl;
	isRunning = false;
	//唤醒所有在等待的线程
	pthread_cond_broadcast(&condition_);//广播

	for (int i = 0; i < threadsNum_; i++)
	{
		pthread_join(threads_[i], NULL);//join所有线程
	}
	delete[] threads_;
	threads_ = NULL;//防止悬挂指针

	pthread_mutex_destroy(&mutex_);
	pthread_cond_destroy(&condition_);
}

int Threadpool::size()
{
	pthread_mutex_lock(&mutex_);//加锁
	int size = taskQueue_.size();
	pthread_mutex_unlock(&mutex_);//解锁
	return size;
}
