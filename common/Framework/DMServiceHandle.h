#pragma once

#include "amqpcpp/amqpcpp.h"
#include "ace/SOCK_Stream.h"
#include "ace/Svc_Handler.h"
//�ײ��װ��Ϣ���պ���Ϣ����
class DMService : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>
{
public:
	virtual void handle(const AMQP::Message &message) = 0;
private:
};
