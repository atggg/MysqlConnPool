#pragma once
#include<mysql.h>
#include<string>
#include<map>
#include<vector>
#include<iostream>

class MysqlConn;
class MysqlRes;
class MysqlValue;

class MysqlConn
{
public:
	bool init();
	bool connect(std::string ip,std::string user,std::string password,std::string dbname,int port = 3306);
	bool setChat(std::string chars);
	bool useDb(std::string dbname);
	bool update(std::string sql);
	std::shared_ptr<MysqlRes> query(std::string sql);
	std::string error();
private:
	MYSQL _conn;
};

class MysqlValue
{
public:
	MysqlValue(std::string val);
	int asInt();
	std::string asString();
private:
	std::string _data;
};

class MysqlRes
{
public:
	MysqlRes(MYSQL_RES* res);
	~MysqlRes();
	MysqlValue value(int row, std::string colName);
	MysqlValue value(int row, int col);
	unsigned int getColCount();
	unsigned int getRowCount();
private:
	MYSQL_RES* _res;
	std::map<std::string, int> _colName;
	std::vector<MYSQL_ROW> _rowlist;
};
