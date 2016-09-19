#include "DMMessageRouter.h"

void DMMessageRouter::send(DMMessage& message, string exchange)
{
    //��·�ɱ�������Ҫά�����û�·�ɱ���Ϣ,��Ҫά��redis�ڴ����ݺ�mysql����
    route(message, exchange);
}

void DMMessageRouter::publish(DMMessage& message)
{
    route(message, FANOUT);
}

DM_BOOL DMMessageRouter::receive(ACE_HANDLE fd, DMMessage& message)
{
    DM_CHAR head[HEAD_CHAR_LEN] = {0};
    ACE_SOCK_Stream stream(fd);
	if (stream.recv(head,HEAD_CHAR_LEN) < 1)
    {
        user_disconnect(fd);
        return false;
    }   
	
	DMMessageParser parser;
	DMMessageHead head_info;
	//parse head
	head_info = parser.parse(head);

    //some message maybe have no message body
    do
    {
    	if ( head_info.length <= 0 || head_info.length > DM_MAX_MEMORY_BLOCK)
    	{
    		break;
    	}

    	//recive body
    	message.require_body_size(head_info.length);
    	memset(message.body,0,head_info.length);
    	stream.recv(message.body,head_info.length);
        
    }while(false);

	message.head = head_info;

	return true;
}

DM_BOOL DMMessageRouter::receive(DMMessage& message, const AMQP::Message& mq_message)
{
	DMMessageParser parser;
	//parse head
	parser.parse(message, mq_message);
    
    return true;
}

void DMMessageRouter::route(DMMessage& message, string exchange)
{
    map<DM_INT32, MsgRange> message_map = DMServiceMap::instance()->message_map;
    map<DM_INT32, MsgRange>::iterator it = message_map.begin();

    DM_INT32 svr_id = 0;
    for (; it != message_map.end(); ++it)
    {
        MsgRange range = it->second;
        if (message.head.msg_cmd > range.msg_start &&
                message.head.msg_cmd < range.msg_end)
        {       
            svr_id = it->first;
            break;
        }
    }

    if (0 != svr_id)
    {   
        //��message���͵�rabbitmq-server��ѡ����Ϣ���ٵĶ���
        route_distribute(message, svr_id, exchange);
    }
}

void DMMessageRouter::route_distribute(DMMessage& message, DM_INT32 service_id, string exchange)
{
    DMMessageParser parser;
    //pack msg
    DM_CHAR *buf = new DM_CHAR[HEAD_CHAR_LEN + message.head.length];
    parser.pack(message,buf);
    
    //��Ϣֱ�Ӹ���ӳ����ָ��cluster��node����
    map<DM_INT32, vector<string>> queue_map = DMServiceMap::instance()->queue_map;
    vector<string> queue = queue_map[service_id];
    vector<string>::iterator it = queue.begin();
    //��һ��������Ϊ��ʼ��ֵ
    string queueName = *it;
    DM_INT32 msgCount = DMMessageQueue::instance()->getQueueMsgCount(queueName);

    for (; it != queue.end(); ++it)
    {
        DM_INT32 size = DMMessageQueue::instance()->getQueueMsgCount(queueName);
        if (msgCount > size)
        {
            queueName = *it;
            msgCount = size;
        }
    }
    
    DMMessageQueue::instance()->publish(exchange, queueName, buf, HEAD_CHAR_LEN + message.head.length);

    delete[] buf;
}

void DMMessageRouter::user_connect(ACE_HANDLE fd, short uid)
{
    DMSessionMgr::instance()->add_session(fd, uid, new DMSession());//fd��Ϊsessionid
}

void DMMessageRouter::user_disconnect(ACE_HANDLE fd)
{
    DMSessionMgr::instance()->del_session(fd);//fd��Ϊsessionid
}

