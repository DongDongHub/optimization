#pragma once
#include "ProxyServiceHandle.h"
#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"

//�˴�����ʹ��ACE_Cached_Connect_Strategy���Ż�
typedef ACE_Acceptor<ProxyServiceHandle, ACE_SOCK_ACCEPTOR> ProxyAcceptor;
