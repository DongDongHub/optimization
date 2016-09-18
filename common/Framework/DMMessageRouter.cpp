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
    DM_CHAR *buf = new DM_CHAR[HEAD_DM_CHAR_LEN + message.head.length];
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
    
    DMMessageQueue::instance()->publish(exchange, queueName, buf, HEAD_DM_CHAR_LEN + message.head.length);

    delete[] buf;
}

