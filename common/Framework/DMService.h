#pragma once

#include "DMaker.h"
//�ײ��װ��Ϣ���պ���Ϣ����
class DMService : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>
{
public:
	virtual void handle(const AMQP::Message &message) = 0;
private:
};
