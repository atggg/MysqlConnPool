#include "MysqlConnPool.h"


MysqlConnPool::MysqlConnPool(int min, int max, std::string ip, std::string user, std::string pwd, std::string dbname, std::string chars, int port):ConnPool(min,max)
{
    _ip = ip;
    _user = user;
    _password = pwd;
    _dbname = dbname;
    _port = port;
    _char = chars;
}

MysqlConn* MysqlConnPool::connect()
{
    MysqlConn *Conn = new MysqlConn();
    Conn->init();
    Conn->connect(_ip, _user, _password, _dbname,_port);
    Conn->setChat(_char);
    return Conn;
}
