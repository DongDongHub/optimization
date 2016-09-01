#include "ReactorPool.h"
#include "ace/Select_Reactor.h"
#include <ace/Log_Msg.h>
#include "ace/Dev_Poll_Reactor.h"
#include "ace/Select_Reactor.h"
#include "ace/Thread_Manager.h"

#define _USE_EPOLL_REACTOR_

void ReactorPool::init(int reactor_count, int handle_max)
{
	ACE_Reactor* pReactor;
	_reactor_pool.reserve(reactor_count);
#ifdef _USE_EPOLL_REACTOR_
	int handle_total = reactor_count * handle_max;
#endif
	while(reactor_count--)
	{
#ifdef _USE_EPOLL_REACTOR_
		pReactor = new ACE_Reactor(new ACE_Dev_Poll_Reactor(handle_total));
#else
		pReactor = new ACE_Reactor(new ACE_Select_Reactor(handle_max));//FD_ISSET����FD_SIZE����飬�޷�����1024��ͬʱhandle����Ҳ��������1024
#endif
		_reactor_pool.push_back(pReactor);
	}
}

ACE_Reactor * ReactorPool::pull()	//��ѯ����reactor
{
	_mutex_lock.acquire();

	static unsigned int i = 0;

	if (!(i < _reactor_pool.size()))
	{
		i = 0;
	}

	ACE_Reactor * pReactor = _reactor_pool[i++];

	_mutex_lock.release();

	return pReactor;
}

void* run_event(void *arg)
{
	ACE_Reactor *reactor = static_cast<ACE_Reactor *> (arg);
	reactor->owner(ACE_OS::thr_self());
	reactor->run_reactor_event_loop();
	return 0;
}

void ReactorPool::run_event_loop(int reactor_thread)//handle��̯���߳���event_loop����Ӧ��hadle_inputΪ���߳�ִ��
{
	if (0 == _reactor_pool.size())
	{
		ACE_DEBUG((LM_INFO,"reactor count = 0,exit!\n"));
	}

	if (1 == _reactor_pool.size())
	{
		ACE_Thread_Manager::instance()->spawn_n(reactor_thread, run_event, _reactor_pool[0]);
	}
	else
	{
		for (unsigned int i = 0; i < _reactor_pool.size(); i++)
		{
			ACE_Thread_Manager::instance()->spawn_n(reactor_thread, run_event, _reactor_pool[i]);//�߳�����Ϊreactor_thread * _reactor_pool.size
		}
	}

	ACE_Thread_Manager::instance()->spawn_n(1, run_event, ACE_Reactor::instance());//accptor��ռ�߳�
	ACE_Thread_Manager::instance()->wait();
}
