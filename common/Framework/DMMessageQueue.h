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
			_instance = DM_NEW() DMMessageQueue();
		}
		return _instance;
	}
    //�������ó��Դ������ж��У�ȡ��svrid���
	DM_BOOL init(string host, DM_UINT16 port, string username,string userpasswd,string serviceName);

	void publish(const string &exchange, const string &routingKey, const DM_CHAR *message, size_t size);

    DM_UINT getQueueMsgCount(string queueName);

    void runEvents();

	void destroy();
	//AMQP::Channel& getChannel();
	//AMQP::Exchange AgetExchange();
protected:
	DMMessageQueue(){};
	virtual ~DMMessageQueue();
private:
    DM_UINT8 _service_id;
    
	DMMessageEvent* _handle;
    
	AMQP::TcpChannel* _channel;
    
	AMQP::TcpConnection* _connection;
    
	static DMMessageQueue *_instance;
};
