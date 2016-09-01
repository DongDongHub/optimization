#include "DMBrokerProxy.h"
#include "ace/Reactor.h"
#include "DMService.h"
#include <ace/Log_Msg.h>
#include <DMServiceMap.h>
#include <map>
extern DMService* GetService();

DMBrokerProxy* DMBrokerProxy::_instance = nullptr;

DM_INT32 DMBrokerProxy::init(string host,DM_INT32 port, string username, string userpasswd,string serviceName)
{
	// create an instance of your own tcp handler
	_handle = new DMBrokerMessageHandle();

	// address of the server
	AMQP::Address address(host, port,AMQP::Login(username,userpasswd),"/");

	// create a AMQP connection object
	_connection = new AMQP::TcpConnection(_handle, address);

	// and create a channel
	_channel = new AMQP::TcpChannel(_connection);

    //get service id
    map<string, DM_INT32> service_map = DMServiceMap::instance()->service_map;
    _service_id = service_map[serviceName];

	auto receiveMessageCallback = [=](const AMQP::Message &message,
		uDM_INT3264_t deliveryTag,
		DM_BOOL redelivered)
	{
		GetService()->receive(message);
		_channel->ack(deliveryTag); //ack rabbitmq-server
	};

	AMQP::QueueCallback callback =
		[=](const string &queue_name, DM_INT32 msgcount, DM_INT32 consumercount)
	{
	    //exchange��targetqueue��routingkey,�������У�����exchange�����ж��У�·�ɼ���������
		_channel->bindQueue("fanout", queue_name, queue_name);
        
		_channel->bindQueue("direct", queue_name, queue_name);
        
		_channel->bindQueue("topic", queue_name, queue_name);

        //���Ѷ���,ֻ���ѱ�server��Ӧ�Ķ���
	    map<DM_INT32, vector<string>> queue_map = DMServiceMap::instance()->queue_map;
        vector<string> consume_queue = queue_map[_service_id];

        for (unsigned DM_INT32 i = 0; i < consume_queue.size(); ++i)
        {
            if (consume_queue[i] == queue_name)
            {
                _channel->consume(queue_name, AMQP::durable).onReceived(receiveMessageCallback);
            }
        }
        
	};

	AMQP::SuccessCallback success = [this, callback]()
	{
	    map<DM_INT32, vector<string>> queue_map = DMServiceMap::instance()->queue_map;
        map<DM_INT32, vector<string>>::iterator svr_it = queue_map.begin();

        for (; svr_it != queue_map.end(); ++svr_it)
        {
            vector<string> queue = svr_it->second;
            for (unsigned DM_INT32 i = 0; i < queue.size(); ++i)
            {
                _channel->declareQueue(queue[i], AMQP::durable).onSuccess(callback);
            }
        }  
        
	};

	//�����������͵�exchange�����ݲ�����Ҫѡ��
	//ֱ��ת�������а󶨵�queue������·�ɼ�ƥ��
	_channel->declareExchange("fanout", AMQP::fanout).onSuccess(success);
    //·�ɼ���ȫƥ��
	_channel->declareExchange("direct", AMQP::direct).onSuccess(success);
    //·�ɼ�ͨ��
	_channel->declareExchange("topic", AMQP::topic).onSuccess(success);

	return 0;
}

void DMBrokerProxy::publish(const string &exchange, const string &routingKey, const DM_CHAR *message, size_t size)
{
	_channel->publish(exchange, routingKey, message, size);
}

DM_INT32 DMBrokerProxy::getQueueMsgCount(string queueName)
{
    DM_INT32 count = 0;
    AMQP::QueueCallback callback =
            [&](const string &queue_name, DM_INT32 msgcount, DM_INT32 consumercount)
    {
        count = msgcount;
    };

    _channel->declareQueue(queueName, AMQP::passive).onSuccess(callback);

    return count;
}

void DMBrokerProxy::runEvents()
{
	ACE_Reactor::instance()->run_event_loop();
}

void DMBrokerProxy::destroy()
{
	if (_handle)
	{
		delete _handle;
		_handle = nullptrptr;
	}

	if (_connection)
	{
		delete _connection;
		_connection = nullptrptr;
	}

	if (_channel)
	{
		delete _channel;
		_channel = nullptrptr;
	}

	if (_instance)
	{
		delete _instance;
		_instance = nullptrptr;
	}
}

DMBrokerProxy::~DMBrokerProxy()
{
	destroy();
}