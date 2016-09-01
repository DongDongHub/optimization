#include "DMMessageRouter.h"

void DMMessageRouter::send(DMMessage& message, string exchange)
{
    //��·�ɱ�������Ҫά�����û�·�ɱ���Ϣ,��Ҫά��redis�ڴ����ݺ�mysql����
    if (0 != message.head.cluster_id && 0 != message.head.node_id)
    {
        string domain = _redis.pack_domain(message.head.user_id, "cluster_id");
        _redis.write_redis_hash("TBL_ROUTE" , domain, message.head.cluster_id);
        
        domain = _redis.pack_domain(message.head.user_id, "node_id");
        _redis.write_redis_hash("TBL_ROUTE" , domain, message.head.node_id);
    }

    route(message, exchange);
}

void DMMessageRouter::publish(DMMessage& message)
{
    route(message, FANOUT);
}

void DMMessageRouter::route(DMMessage& message, string exchange)
{
    //�������ݼ�Ⱥ�ڵ�����ָ��·��
    if (route_assign(message, exchange))
    {
        return;
    }

    map<DM_INT32, MsgRange> message_map = DMServiceMap::instance()->message_map;
    map<DM_INT32, MsgRange>::iterator it = message_map.begin();

    DM_INT32 svr_id = 0;
    for (; it != message_map.end(); ++it)
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

DM_BOOL DMMessageRouter::route_assign(DMMessage& message, string exchange)
{
    DMMessageParser parser;
    //pack msg
    DM_CHAR *buf = DM_NEW() DM_CHAR[HEAD_DM_CHAR_LEN + message.head.length];
    parser.pack(message,buf);
    
    string domain = _redis.pack_domain(message.head.user_id, "cluster_id");
    string cluster = _redis.read_redis_hash("TBL_ROUTE",domain);
    
    domain = _redis.pack_domain(message.head.user_id, "node_id");
    string node = _redis.read_redis_hash("TBL_ROUTE",domain);

    if (cluster == "nil" || node == "nil")
    {
        //��ѯmysql
    }

    //ָ�����ͣ��ڵ㴦��ʽ�����¿���
    DMMessageQueue::getInstance()->publish(exchange, cluster, buf, HEAD_DM_CHAR_LEN + message.head.length);
    return TRUE;
}

void DMMessageRouter::route_distribute(DMMessage& message, DM_INT32 service_id, string exchange)
{
    DMMessageParser parser;
    //pack msg
    DM_CHAR *buf = DM_NEW() DM_CHAR[HEAD_DM_CHAR_LEN + message.head.length];
    parser.pack(message,buf);
    
    //��Ϣֱ�Ӹ���ӳ����ָ��cluster��node����
    map<DM_INT32, vector<string>> queue_map = DMServiceMap::instance()->queue_map;
    vector<string> queue = queue_map[service_id];
    vector<string>::iterator it = queue.begin();
    //��һ��������Ϊ��ʼ��ֵ
    string queueName = *it;
    DM_INT32 msgCount = DMMessageQueue::getInstance()->getQueueMsgCount(queueName);

    for (; it != queue.end(); ++it)
    {
        DM_INT32 size = DMMessageQueue::getInstance()->getQueueMsgCount(queueName);
        if (msgCount > size)
        {
            queueName = *it;
            msgCount = size;
        }
    }
    
    DMMessageQueue::getInstance()->publish(exchange, queueName, buf, HEAD_DM_CHAR_LEN + message.head.length);

    DM_DELETE()[] buf;
}

