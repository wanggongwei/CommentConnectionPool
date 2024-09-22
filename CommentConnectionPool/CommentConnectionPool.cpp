// CommentConnectionPool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include "pch.h"
#include "CommentConnectionPool.h"
#include <chrono>
//线程安全的懒汉式单例函数接口

//构造函数
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
		//更新进入队列时间
		p->refreshAliveTime();
		_connectionQueue.push(p);
		_curConnectionSize++;
	}
	//启动一个新的线程，作为连接生产者
	std::thread produce(std::bind(&ConnectionPool::produceConnTask,this));
	produce.detach();
	//启动定时线程，检查动态创建的连接是否空闲超时
	std::thread checkTimeout(std::bind(&ConnectionPool::checkIdleTimeLink, this));
	checkTimeout.detach();
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
				//更新进入队列时间
				p->refreshAliveTime();
				_connectionQueue.push(p);
				_curConnectionSize++;
				//通知所有等待在条件变量的线程可以消费连接
				_connQueue_not_empty.notify_all();
			}
		}
		
	}	
}
//检查空闲连接是否超时需要回收
void ConnectionPool::checkIdleTimeLink() 
{
	//队列前端连接空闲最久，检查超时则出队并析构
	while (1) 
	{
		std::this_thread::sleep_for(std::chrono::seconds(10));
		std::unique_lock<std::mutex> lock(_queueMutex);
		while (_curConnectionSize > _initSize) 
		{
			Connection* p = _connectionQueue.front();
			if (p->getAliveTime() > (_maxIdleTime * 1000))
			{
				_connectionQueue.pop();
				_curConnectionSize--;
				delete p;	//释放连接
			}
			else 
			{
				break;
			}
		}
		
	}
}
//给外部提供结构，获取线程池实例
ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool;	//由编译器lock和unlock
	return &pool;
}

//外部提供接口从连接队列中取得连接
std::shared_ptr<Connection> ConnectionPool::getConnection() 
{
	std::unique_lock<std::mutex> lock(_queueMutex);
	//当连接队列为空
	while (_connectionQueue.empty()) 
	{
		//获取连接，等待一定时间
		auto waitRes = _connQueue_not_empty.wait_for(lock,std::chrono::milliseconds(_connectionTimeOut));
		//获取超时，则打印获取失败
		if (_connectionQueue.empty()) 
		{
			if (waitRes == std::cv_status::timeout) 
			{
				LOG("get link time out !,get failed");
				return nullptr;
			}
		}
	}
	/*
	shared_ptr 析构时会将connection资源直接delete掉，
	相当于调用connection的析构函数，connection就被close掉
	这里自定义shared_ptr的释放资源方式，把connection直接归还给连接队列
	*/
	std::shared_ptr<Connection> res(_connectionQueue.front(), [&](Connection* pcon) {
		//在服务器应用线程中调用，需要考虑队列的线程安全操作
		std::unique_lock<std::mutex> lock(_queueMutex);
		//更新进入队列时间
		pcon->refreshAliveTime();
		_connectionQueue.push(pcon);
		});
	_connectionQueue.pop();

	_connQueue_not_empty.notify_all(); //消费完成，通知可以生产或其他线程消费

	return res;


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
