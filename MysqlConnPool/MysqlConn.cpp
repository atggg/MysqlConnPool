#include "MysqlConn.h"

bool MysqlConn::init()
{
	void * rc =  mysql_init(&_conn);
	return rc != nullptr;
}

bool MysqlConn::connect(std::string ip, std::string user, std::string password, std::string dbname, int port)
{
	void* rc = mysql_real_connect(&_conn, ip.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);
	return rc != nullptr;
}

bool MysqlConn::setChat(std::string chars)
{
	int rc = mysql_set_character_set(&_conn, chars.c_str());
	return rc == 0;
}

bool MysqlConn::useDb(std::string dbname)
{
	int rc = mysql_select_db(&_conn, dbname.c_str());
	return false;
}

bool MysqlConn::update(std::string sql)
{
	int rc = mysql_query(&_conn, sql.c_str());
	
	return rc == 0;
}

std::shared_ptr<MysqlRes> MysqlConn::query(std::string sql)
{
	int rc = mysql_query(&_conn, sql.c_str());
	if(rc)
	{
		return std::shared_ptr<MysqlRes>(nullptr);
	}
	MYSQL_RES* res = mysql_store_result(&_conn);
	return std::shared_ptr<MysqlRes>(new MysqlRes(res));
}


std::string MysqlConn::error()
{
	return mysql_error(&_conn);
}

MysqlRes::MysqlRes(MYSQL_RES* res)
{
	_res = res;
	for (int i = 0; i < getColCount(); i++)
	{
		_colName.insert(std::make_pair(mysql_fetch_field(_res)->name,i));
	}
	for (int i = 0; i < getRowCount(); i++)
	{
		_rowlist.push_back(mysql_fetch_row(_res));
	}
}

MysqlRes::~MysqlRes()
{
	mysql_free_result(_res);
}

MysqlValue MysqlRes::value(int row, std::string colName)
{
	int col = _colName.find(colName)->second;
	return MysqlValue(std::string(_rowlist[row][col],mysql_fetch_lengths(_res)[col]));
}

MysqlValue MysqlRes::value(int row, int col)
{
	if (_rowlist[row][col] == nullptr)
	{
		return MysqlValue(std::string("NULL"));
	}
	return MysqlValue(std::string(_rowlist[row][col], mysql_fetch_lengths(_res)[col]));
}

unsigned int MysqlRes::getColCount()
{
	return mysql_num_fields(_res);
}

unsigned int MysqlRes::getRowCount()
{
	return mysql_num_rows(_res);

}

MysqlValue::MysqlValue(std::string val)
{
	_data = val;
}

int MysqlValue::asInt()
{
	return std::stoi(_data,nullptr,10);
}

std::string MysqlValue::asString()
{
	return _data;
}
