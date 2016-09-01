#pragma once
#include "ace/Reactor.h"
#include "ace/Singleton.h"
#include <vector>

class ReactorPool
{
public:
	void init(int reactor_count, int handle_max);

	//ȡ��һ�����õ�reactor�� һ��reactor���ƹ���1000��handle
	ACE_Reactor* pull();

	void run_event_loop(int reactor_thread);
protected:
	//��ʼ���̶�������reactor
	std::vector<ACE_Reactor*> _reactor_pool;
	ACE_Thread_Mutex _mutex_lock;
	//ACE_Reactor** _pool;
private:
};

typedef ACE_Singleton<ReactorPool, ACE_Thread_Mutex> Reactor_Pool; 
