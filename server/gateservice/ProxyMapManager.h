#pragma once
//#include "ace/RW_Mutex.h"
#include <map>

//ƥ�����Ϊaccountid�����һλ(���ֻ����ַ�)��Ӧһ��proxy.

class ProxyMapManager
{
public:
	static ProxyMapManager* getInstance();
	std::string hash(char accountid);

	//��������
	void update(char key, std::string proxy);
	//ɾ������
	void remove(char key);
	//�������ļ��ж�ȡ���ù�ϵ
	void load();

protected:
	ProxyMapManager();


private:
	static ProxyMapManager* _instance;

	std::map<char, std::string> _map;

	//ACE_RW_Mutex _mutex;
};

