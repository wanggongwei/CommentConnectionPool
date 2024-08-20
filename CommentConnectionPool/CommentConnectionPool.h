#pragma once
/*
实现连接池功能模块
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
	//获取连接池对象实例
	static ConnectionPool* getConnectionPool();
	//给外部提供结构从连接池中获取一个可用的空闲连接
	std::shared_ptr<Connection> getConnection();
private:
	ConnectionPool();//单例#1，构造函数私有化

	bool loadConfigFile();	//从配置文件中加载配置项

	void produceConnTask();//运行在独立的线程中，专门负责生产新连接
	std::string _ip;  //mysql的ip地址
	unsigned short _port;	//mysql端接口 3306
	std::string _username;	//mysql用户名
	std::string _password;	//mysql登录密码
	std::string _dbname; //所连接的数据库名称
 	int _initSize;	//连接池初始连接数量
	int _maxSize;	//连接池最大连接数量
	int _maxIdleTime;	//连接池最大空闲时间
	int _connectionTimeOut;	//连接池获取连接的超时时间

	std::queue<Connection*> _connectionQueue; //存储mysql连接的队列
	std::mutex _queueMutex;	//维护连接队列的线程安全互斥锁	
	std::atomic_int _curConnectionSize;	//用来记录所创建的connection的总数量
	std::condition_variable _connQueue_not_empty;//设置条件变量用于连接生产线程和连接消费线程的通信


};