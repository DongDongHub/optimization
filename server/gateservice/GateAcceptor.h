#pragma once
#include "GateServiceHandle.h"
#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"

//�˴�����ʹ��ACE_Cached_Connect_Strategy���Ż�
typedef ACE_Acceptor<GateServiceHandle, ACE_SOCK_ACCEPTOR> GateAcceptor;
