#include"ThreadPool.h"
#include <unistd.h>
#include<string>

//全局函数可以做任务
void run1(int index, string name, void* ptr) {
	cout << "task with name " << name << " is running,index is" << index << endl;
	cout << "processing ptr " << ptr << " ..." << endl;
	sleep(5);
}

//class也可以做任务
class MyTask {
public:
	void run2(int index, string name, void* ptr) {
		cout << "task with name " << name << " is running,index is" << index << endl;
		cout << "processing ptr " << ptr << " ..." << endl;
		sleep(5);
	}

};

int main() {
	Threadpool pool(5);

	function<void()> task1 = bind(run1, 1, "read", nullptr);
	MyTask task;
	function<void()> task2 = bind(&MyTask::run2, &task, 2, "write", nullptr);

	sleep(1);

	pool.addTask(task1);
	pool.addTask(task2);

	sleep(1);

	pool.stop();
}