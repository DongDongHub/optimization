#pragma once

#include "DMaker.h"
//�ײ��װ��Ϣ���պ���Ϣ����
class DMService// : public ACE_Task<ACE_MT_SYNCH>
{
public:
	virtual void handle(const AMQP::Message &message);
private:
};
