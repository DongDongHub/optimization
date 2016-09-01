#pragma once
#include "DMMessage.h"
#include "DMServiceMap.h"
#include "DMRedis.h"
#include "DMMySQL.h"

//��Ϣ·����������������Ϣ·�ɵ���Ϣ������
//������ͨ����ֱ��ʹ�þͿɡ� ����proxyServer����Ҫ���أ����·�ɵ��ͻ��˵�������

#define FANOUT "fanout"
#define DIRECT "direct"
#define TOPIC "topic"

class DMRouter
{
public:
    virtual void send(DMMessage& message, std::string exchange = "direct");

    virtual void publish(DMMessage& message);
        
protected:
    
	virtual void route(DMMessage& message, std::string exchange);
            
private:

    bool route_assign(DMMessage& message, std::string exchange);

    void route_distribute(DMMessage& message, int service_id, std::string exchange);

private:
    DMRedis _redis;
    DMMySQL _mysql;
};

