#pragma once
#include"MysqlConn.h"
#include"ConnPool.hpp"
class MysqlConnPool:public ConnPool<MysqlConn>
{
public:
	using ConnPool::ConnPool;
	MysqlConnPool(int min,int max,std::string ip,std::string user,std::string pwd,std::string dbname,std::string chars = "utf8", int port = 3306);
private:
	virtual MysqlConn* connect();
	std::string _ip;
	std::string _user;
	std::string _password;
	std::string _dbname;
	std::string _char;
	int _port;

};

