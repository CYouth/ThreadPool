#ifndef TASK
#define TASK

#include<string>
using namespace std;
//����ӿ�
class Task
{
public:
	virtual void run() = 0;//���麯��

//���������ִ�в������Է�������
protected:
	string name_;
	int index_;
};


#endif // !TASK
