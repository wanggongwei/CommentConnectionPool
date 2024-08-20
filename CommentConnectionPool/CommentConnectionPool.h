#pragma once
/*
ʵ�����ӳع���ģ��
*/
#include <string>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>
#include <functional>
#include "public.h"
#include "Connection.h"
class ConnectionPool
{
public:
	//��ȡ���ӳض���ʵ��
	static ConnectionPool* getConnectionPool();
	//���ⲿ�ṩ�ṹ�����ӳ��л�ȡһ�����õĿ�������
	std::shared_ptr<Connection> getConnection();
private:
	ConnectionPool();//����#1�����캯��˽�л�

	bool loadConfigFile();	//�������ļ��м���������

	void produceConnTask();//�����ڶ������߳��У�ר�Ÿ�������������
	std::string _ip;  //mysql��ip��ַ
	unsigned short _port;	//mysql�˽ӿ� 3306
	std::string _username;	//mysql�û���
	std::string _password;	//mysql��¼����
	std::string _dbname; //�����ӵ����ݿ�����
 	int _initSize;	//���ӳس�ʼ��������
	int _maxSize;	//���ӳ������������
	int _maxIdleTime;	//���ӳ�������ʱ��
	int _connectionTimeOut;	//���ӳػ�ȡ���ӵĳ�ʱʱ��

	std::queue<Connection*> _connectionQueue; //�洢mysql���ӵĶ���
	std::mutex _queueMutex;	//ά�����Ӷ��е��̰߳�ȫ������	
	std::atomic_int _curConnectionSize;	//������¼��������connection��������
	std::condition_variable _connQueue_not_empty;//�������������������������̺߳����������̵߳�ͨ��


};