#pragma once
/*
	ʵ��mysql���ݿ����ɾ�Ĳ�Ȳ���
*/
#include <mysql.h>


class Connection
{
public:
	Connection();
	~Connection();

	//�������ݿ�
	bool connect(std::string ip, unsigned short port, std::string user, std::string password, std::string dbname);
	//���²���
	bool update(std::string sql);
	//��ѯ����
	MYSQL_RES* query(std::string sql);
private:
	MYSQL* _conn; //��ʾ��MySQL Server��һ������
};