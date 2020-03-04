#include"ThreadPool.h"
#include <unistd.h>

//具体的任务类，可以用工厂模式根据需要产生
class MyTask:public Task
{
public:
	MyTask(int index,string name) {
		index_ = index;
		name_ = name;
	}
	void setArgs(void* ptr) {
		ptr_ = ptr;
	}
	void run() {
		cout << "task with name "<<name_<< " is running,index is" << index_ << endl;
		cout << "processing ptr " << ptr_ << " ..." << endl;
		sleep(5);
	}
private:
	void* ptr_;
};

int main() {
	Threadpool pool(5);
	MyTask task1(1, "read");
	task1.setArgs(NULL);
	MyTask task2(2, "write");
	task2.setArgs(NULL);
	MyTask task3(3, "openfile");
	task3.setArgs(NULL);

	sleep(1);

	pool.addTask(&task1);
	pool.addTask(&task2);
	pool.addTask(&task3);

	sleep(1);

	pool.stop();
}