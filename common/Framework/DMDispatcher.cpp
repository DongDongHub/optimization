#include "DMDispatcher.h"

DMDispatcher::DMDispatcher()
{
    _msg_queue = DMMessageQueue::instance();
    _msg_queue->register_dispatcher(this);
}

//���ͷ���������ӹ���
int DMDispatcher::handle_input(ACE_HANDLE fd)
{
    return 0;
}

int DMDispatcher::handle_input(const AMQP::Message &message)
{
    return 0;
}

void DMDispatcher::init(DMService* pService)
{
    _service = pService;
    _msg_queue->init();
}
