#ifndef TASK
#define TASK

#include<string>
using namespace std;
//任务接口
class Task
{
public:
	virtual void run() = 0;//纯虚函数

//关于任务的执行参数可以放在这里
protected:
	string name_;
	int index_;
};


#endif // !TASK
