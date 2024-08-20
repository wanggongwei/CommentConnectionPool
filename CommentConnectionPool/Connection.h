#pragma once
/*
	实现mysql数据库的增删改查等操作
*/
#include <mysql.h>


class Connection
{
public:
	Connection();
	~Connection();

	//连接数据库
	bool connect(std::string ip, unsigned short port, std::string user, std::string password, std::string dbname);
	//更新操作
	bool update(std::string sql);
	//查询操作
	MYSQL_RES* query(std::string sql);
private:
	MYSQL* _conn; //表示和MySQL Server的一条连接
};