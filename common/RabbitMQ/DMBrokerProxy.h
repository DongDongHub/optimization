#pragma once
#include "amqpcpp.h"
#include "DMBrokerMessageHandle.h"

//rabitmq���������Ӵ�����service��dispatchʹ��
class DMBrokerProxy
{
public:
	static DMBrokerProxy* getInstance()
	{
		if (NULL == _instance)
		{
			_instance = new DMBrokerProxy();
		}
		return _instance;
	}
    //�������ó��Դ������ж��У�ȡ��svrid���
	int init(std::string host, int port, std::string username,std::string userpasswd,std::string serviceName);

	void publish(const std::string &exchange, const std::string &routingKey, const char *message, size_t size);

    int getQueueMsgCount(std::string queueName);

    void runEvents();

	void destroy();
	//AMQP::Channel& getChannel();
	//AMQP::Exchange AgetExchange();
protected:
	DMBrokerProxy(){};
	virtual ~DMBrokerProxy();
private:
    int _service_id;
    
	DMBrokerMessageHandle* _handle;
    
	AMQP::TcpChannel* _channel;
    
	AMQP::TcpConnection* _connection;
    
	static DMBrokerProxy *_instance;
};
