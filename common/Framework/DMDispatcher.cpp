#include "DMDispatcher.h"

DMDispatcher::DMDispatcher()
{
    _msg_queue = DMMessageQueue::instance();
    _msg_queue->register_dispatcher(this);
}

//���ͷ���������ӹ���
virtual int DMDispatcher::handle_input(ACE_HANDLE fd)
{
    _service
}

virtual int DMDispatcher::handle_input(const AMQP::Message &message)
{

}

