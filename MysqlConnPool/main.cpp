#include"MysqlConnPool.h"
class A
{

};
int main()
{
	
	std::pair<int, std::string> d(1, "h");
	d.first;

	ConnPool<MysqlConn>* Pool = new MysqlConnPool(4, 8, "127.0.0.1", "root", "", "test1");
	Pool->start();
	std::shared_ptr<MysqlConn> Conn =  Pool->getConn();
	auto Res =  Conn->query("select * from user_1");
	for (int i = 0; i < Res->getRowCount(); i++)
	{
		for (int j = 0; j < Res->getColCount(); j++)
		{
			std::cout << Res->value(i, j).asString() << "	";
		}
		std::cout << std::endl;
	}
	
}

