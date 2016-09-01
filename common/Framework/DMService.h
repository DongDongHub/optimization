#pragma once
#include "amqpcpp/include/message.h"
#include "DMServiceHandle.h"
#include "DMRouter.h"

//�������Ա�ṹ���ɵ������У�ҵ������ȡ����ģʽ�����ʹ�����ϣ�DMServiceHandleΪ�������,������ά��������Ҳ�ǵ�ҵ��㣬���ɵײ����װ���δ���
//������Ӧ�ô���������Ӧ�ò�֮�䣬��Ҫ��װ�ײ�IO��dispatch��routeӦ���ڸ��ײ㣬����dispatch��һ��
//dispatch���˰���route������Ҫ����һ����Ϣ�����������Ϣ������ҵ����ṩ����������Ϣ����ע�����Ϣͷ�����proto���򡣶�ҵ��ֱ���ṩ�ɿ�����
//�ɴˣ�ҵ����ֻ��Ҫ�ṩҵ��handle���Ժ���Ϣ�������ɣ��������������ṩһ�������ĵײ���������࣬�ϲ�ֻ��Ҫ���л�����ע���������
//����ACE��MQ�ӿھ��ڵײ㴦��ת�ӵ�ҵ���ϵĽӿ������ڴ˻���������������
//���ݿ�������������ӿڵ������ڣ���ȫ����ͨ�ã��������ʽ���õ�ҵ����

template <typename SERVICE_HANDLE>
class DMServer
{
public:
    //������
    typedef ACE_Acceptor<SERVICE_HANDLE, ACE_SOCK_ACCEPTOR> DMAcceptor;
    //��ʼ����������
	int init();
    
	void run();

	void register_service(DMService* service);
    
	void register_message_factory(DMMessageFactory* handle) { _handle = handle; };
private:
    //IO�㣬�ײ�ͳһ�����������ṩ
    DMAcceptor _acceptor;
    //dispatch�㣬�ײ㶨�壬�����ṩ��Ϣ������ע��
	DMDispatcher _dispatcher;
    //ҵ��㣬����ʵ��
	DMService* _service;
};

