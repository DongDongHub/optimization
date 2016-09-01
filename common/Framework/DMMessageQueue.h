#pragma once
#include "DMaker.h"

//rabitmq���������Ӵ�����service��dispatchʹ��
class DMBrokerProxy
{
public:
	static DMBrokerProxy* getInstance()
	{
		if (nullptr == _instance)
		{
			_instance = new DMBrokerProxy();
		}
		return _instance;
	}
    //�������ó��Դ������ж��У�ȡ��svrid���
	DM_INT32 init(string host, DM_INT32 port, string username,string userpasswd,string serviceName);

	void publish(const string &exchange, const string &routingKey, const DM_CHAR *message, size_t size);

    DM_INT32 getQueueMsgCount(string queueName);

    void runEvents();

	void destroy();
	//AMQP::Channel& getChannel();
	//AMQP::Exchange AgetExchange();
protected:
	DMBrokerProxy(){};
	virtual ~DMBrokerProxy();
private:
    DM_INT32 _service_id;
    
	DMBrokerMessageHandle* _handle;
    
	AMQP::TcpChannel* _channel;
    
	AMQP::TcpConnection* _connection;
    
	static DMBrokerProxy *_instance;
};
