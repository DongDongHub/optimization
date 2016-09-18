//=============================================================================
/* 
*  File: DMMessageRouter.h
*
*  Author: bing
*
*  Date: 2016-09-01
*
*  Version: v2.0
*
*  Github/Mail: https://github.com/binchen-china    <563853086@qq.com>
*
*  Note:
*/
//=============================================================================

#pragma once
#include "DMaker.h"
#include "DMMessageParser.h"
#include "DMMessageQueue.h"

//��Ϣ·����������������Ϣ·�ɵ���Ϣ������
//������ͨ����ֱ��ʹ�þͿɡ� ����proxyServer����Ҫ���أ����·�ɵ��ͻ��˵�������

#define FANOUT "fanout"
#define DIRECT "direct"
#define TOPIC "topic"

class DMMessageRouter
{
public:
    virtual void send(DMMessage& message, string exchange = "direct");

    virtual void publish(DMMessage& message);
    
    virtual DM_BOOL receive(ACE_HANDLE fd, DMMessage& message);

    virtual DM_BOOL receive(DMMessage& message, const AMQP::Message& mq_message);
   
protected:
    
	virtual void route(DMMessage& message, string exchange);
    
private:

    void route_distribute(DMMessage& message, DM_INT32 service_id, string exchange);
    
    void user_connect(ACE_HANDLE fd, short uid);
    
    void user_disconnect(ACE_HANDLE fd);

};

