#include "pch.h"
#include "public.h"
#include "Connection.h"


Connection::Connection() {
	_conn = mysql_init(nullptr);
};

Connection::~Connection() {
	//释放数据库连接资源
	if (_conn != nullptr) {
		mysql_close(_conn);
	}
}
bool Connection::connect(std::string ip, unsigned short port, std::string user, std::string password, std::string dbname) 
{
	//连接数据库
	MYSQL* p = mysql_real_connect(_conn, ip.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);
	return p != nullptr;
}

bool Connection::update(std::string sql) 
{
	//更新操作
	if (mysql_query(_conn, sql.c_str()))
	{
		std::cerr << "Update failed: " << sql << std::endl;
		std::cerr << "Error: " << mysql_error(_conn) << std::endl;
		return false;
	}
	return true;
}

MYSQL_RES* Connection::query(std::string sql) 
{
	//查询操作
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("查询失败" + sql);
		return nullptr;
	}
	return mysql_use_result(_conn);
}
