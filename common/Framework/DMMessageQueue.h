#pragma once
#include "DMaker.h"

//rabitmq���������Ӵ�����service��dispatchʹ��
class DMMessageQueue
{
public:
	static DMMessageQueue* getInstance()
	{
		if (nullptr == _instance)
		{
			_instance = DM_NEW(*_instance, sizeof(DMMessageQueue));
		}
		return _instance;
	}
    //�������ó��Դ������ж��У�ȡ��svrid���

    //void register_dispatcher(DMDispatcher* pDispatcher){_dispatcher = pDispatcher};
    
	DM_BOOL init(string host, DM_UINT16 port, string username,string userpasswd,string serviceName);

	void publish(const string &exchange, const string &routingKey, const DM_CHAR *message, size_t size);

    DM_UINT getQueueMsgCount(string queueName);

    void runEvents();

	void destroy();

protected:
	DMMessageQueue(){};
	virtual ~DMMessageQueue();
    
private:
    DM_UINT8 _service_id;
    
	DMMessageEvent* _handle;

    DMDispatcher* _dispatcher;
    
	AMQP::TcpChannel* _channel;
    
	AMQP::TcpConnection* _connection;
    
	static DMMessageQueue *_instance;
};
