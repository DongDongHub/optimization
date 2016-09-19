//=============================================================================
/* 
*  File: DMServer.h
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
#include "DMServerImpl.h"
#include "DMAcceptor.h"
#include "DMDispatcher.h"
#include "DMService.h"

class DMServer : private DMServerImpl
{
public:
    DMServer(DMService* service, DMMessageFactory* factory);

	DM_BOOL init(const std::string& config_path, int argc, char *argv[]);
    
	void run();

private:

    DMAcceptor* _acceptor;
    
	DMDispatcher _dispatcher;
    
	DMService* _service;
};

