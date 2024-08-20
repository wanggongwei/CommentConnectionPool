// CommentConnectionPool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include "pch.h"
#include "CommentConnectionPool.h"
//线程安全的懒汉式单例函数接口
ConnectionPool::ConnectionPool() {
	//加载配置项
	if (!loadConfigFile())
	{
		return;
	}
	/*
		创建初始数量的连接
	*/
	for (int i = 0; i < _initSize; i++) 
	{
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		_connectionQueue.push(p);
		_curConnectionSize++;
	}
	//启动一个新的线程，作为连接生产者
	std::thread produce(std::bind(&ConnectionPool::produceConnTask,this));
}

//运行在独立的线程中，专门负责生产新连接
void ConnectionPool::produceConnTask() 
{
	for (;;) 
	{
		std::unique_lock<std::mutex> lock(_queueMutex);
		while (!_connectionQueue.empty()) 
		{
			_connQueue_not_empty.wait(lock);//队列不用，此处生产者线程进入等待状态，并释放锁
		}
		if (_curConnectionSize < _maxSize)
		{
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			if (p != nullptr) 
			{
				_connectionQueue.push(p);
				_curConnectionSize++;
				//通知所有等待在条件变量的线程可以消费连接
				_connQueue_not_empty.notify_all();
			}
		}
		
	}	
}

ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool;	//由编译器lock和unlock
	return &pool;
}

std::shared_ptr<Connection> ConnectionPool::getConnection() 
{
	return nullptr;
}
//从配置文件中加载配置项
bool ConnectionPool::loadConfigFile() 
{
	FILE* pf;
	errno_t err = fopen_s(&pf,"mysql.ini", "r");
	if (err!=0) {
		LOG("mysql.ini file is not exist! ");
		return false;
	}
	while (!feof(pf)) {
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		std::string str = line;
		int idx = str.find('=', 0);
		if (idx == -1)	//无效配置项
		{
			continue;
		}
		//passowrd=123456\n
		int endidx = str.find('\n', idx);
		std::string key = str.substr(0, idx);
		std::string value = str.substr(idx + 1, endidx - idx - 1);
		
		if (key == "ip") {
			_ip = value;
		}
		else if (key == "port") _port = atoi(value.c_str());
		else if (key == "username") _username = value;
		else if (key == "password") _password = value;
		else if (key == "dbname") _dbname = value;
		else if (key == "initSize") _initSize = atoi(value.c_str());
		else if (key == "maxSize") _maxSize = atoi(value.c_str());
		else if (key == "maxIdleTime") _maxIdleTime = atoi(value.c_str());
		else if (key == "connectionTimeOut") _connectionTimeOut = atoi(value.c_str());
	}
	return true;
}
