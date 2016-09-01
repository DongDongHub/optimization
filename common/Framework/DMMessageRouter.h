#pragma once
#include "DMaker.h"

//��Ϣ·����������������Ϣ·�ɵ���Ϣ������
//������ͨ����ֱ��ʹ�þͿɡ� ����proxyServer����Ҫ���أ����·�ɵ��ͻ��˵�������

#define FANOUT "fanout"
#define DIRECT "direct"
#define TOPIC "topic"

class DMRouter
{
public:
    virtual void send(DMMessage& message, string exchange = "direct");

    virtual void publish(DMMessage& message);
        
protected:
    
	virtual void route(DMMessage& message, string exchange);
            
private:

    DM_BOOL route_assign(DMMessage& message, string exchange);

    void route_distribute(DMMessage& message, DM_UINT service_id, string exchange);

};

