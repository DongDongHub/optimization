#include "DMDispatcher.h"

//���ͷ���������ӹ���
virtual int DMDispatcher::handle_input(ACE_HANDLE fd)
{
    receive_app_msg();
}

virtual int DMDispatcher::handle_input(const AMQP::Message &message)
{
    receive_mq_msg();
}

