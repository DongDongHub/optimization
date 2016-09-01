#include "DMRouter.h"
#include "DMBrokerProxy.h"
#include "DMMessageParser.h"
#include <fstream>
#include "json/json.h"
#include <ace/Log_Msg.h>
#include <map>

void DMRouter::send(DMMessage& message, std::string exchange)
{
    //��·�ɱ�������Ҫά�����û�·�ɱ���Ϣ,��Ҫά��redis�ڴ����ݺ�mysql����
    if (0 != message.head.cluster_id && 0 != message.head.node_id)
    {
        std::string domain = _redis.pack_domain(message.head.user_id, "cluster_id");
        _redis.write_redis_hash("TBL_ROUTE" , domain, message.head.cluster_id);
        
        domain = _redis.pack_domain(message.head.user_id, "node_id");
        _redis.write_redis_hash("TBL_ROUTE" , domain, message.head.node_id);
    }

    route(message, exchange);
}

void DMRouter::publish(DMMessage& message)
{
    route(message, FANOUT);
}

void DMRouter::route(DMMessage& message, std::string exchange)
{
    //�������ݼ�Ⱥ�ڵ�����ָ��·��
    if (route_assign(message, exchange))
    {
        return;
    }

    std::map<int, MsgRange> message_map = DMServiceMap::instance()->message_map;
    std::map<int, MsgRange>::iterator it = message_map.begin();

    int svr_id = 0;
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

bool DMRouter::route_assign(DMMessage& message, std::string exchange)
{
    DMMessageParser parser;
    //pack msg
    char *buf = new char[HEAD_CHAR_LEN + message.head.length];
    parser.pack(message,buf);
    
    std::string domain = _redis.pack_domain(message.head.user_id, "cluster_id");
    std::string cluster = _redis.read_redis_hash("TBL_ROUTE",domain);
    
    domain = _redis.pack_domain(message.head.user_id, "node_id");
    std::string node = _redis.read_redis_hash("TBL_ROUTE",domain);

    if (cluster == "nil" || node == "nil")
    {
        //��ѯmysql
    }

    //ָ�����ͣ��ڵ㴦��ʽ�����¿���
    DMBrokerProxy::getInstance()->publish(exchange, cluster, buf, HEAD_CHAR_LEN + message.head.length);
    return true;
}

void DMRouter::route_distribute(DMMessage& message, int service_id, std::string exchange)
{
    DMMessageParser parser;
    //pack msg
    char *buf = new char[HEAD_CHAR_LEN + message.head.length];
    parser.pack(message,buf);
    
    //��Ϣֱ�Ӹ���ӳ����ָ��cluster��node����
    std::map<int, std::vector<std::string>> queue_map = DMServiceMap::instance()->queue_map;
    std::vector<std::string> queue = queue_map[service_id];
    std::vector<std::string>::iterator it = queue.begin();
    //��һ��������Ϊ��ʼ��ֵ
    std::string queueName = *it;
    int msgCount = DMBrokerProxy::getInstance()->getQueueMsgCount(queueName);

    for (; it != queue.end(); ++it)
    {
        int size = DMBrokerProxy::getInstance()->getQueueMsgCount(queueName);
        if (msgCount > size)
        {
            queueName = *it;
            msgCount = size;
        }
    }
    
    DMBrokerProxy::getInstance()->publish(exchange, queueName, buf, HEAD_CHAR_LEN + message.head.length);

    delete[] buf;
}

