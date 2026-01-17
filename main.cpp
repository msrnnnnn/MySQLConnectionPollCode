#include <iostream>
#include "Connection.h"
#include "MySQLConnectionPool.h"
using namespace std;
 
int main()
{
	clock_t begin = clock();
	
	
	thread t1([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		shared_ptr<Connection> sp = cp->getConnection();
		for (int i = 0; i < 2500; ++i)
		{   
			string sql_str;
			sql_str.resize(1024);
			snprintf(&sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29, "male");
			sp->update(sql_str.c_str());
		}
		});
	thread t2([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		shared_ptr<Connection> sp = cp->getConnection();
		for (int i = 0; i < 2500; ++i)
		{
			string sql_str;
			sql_str.resize(1024);
			snprintf(&sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29, "male");
			sp->update(sql_str.c_str());
		}
		});
	thread t3([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		shared_ptr<Connection> sp = cp->getConnection();
		for (int i = 0; i < 2500; ++i)
		{
			string sql_str;
			sql_str.resize(1024);
			snprintf(&sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29, "male");
			sp->update(sql_str.c_str());
		}
		});
	thread t4([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		shared_ptr<Connection> sp = cp->getConnection();
		for (int i = 0; i < 2500; ++i)
		{
			string sql_str;
			sql_str.resize(1024);
			snprintf(&sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29, "male");
			sp->update(sql_str.c_str());
		}
		});

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;
	return 0;
}

#if 0

for (int i = 0; i < 1000; ++i)
{   /*
	Connection conn;
	string sql_str;
	sql_str.resize(1024);
	snprintf(&sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29, "male");
	conn.connect("127.0.0.1", 3306, "root", "18277274635a", "test");
	conn.update(sql_str.c_str());
	*/
	shared_ptr<Connection> sp = cp->getConnection();
	string sql_str;
	sql_str.resize(1024);
	snprintf(&sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29, "male");
	sp->update(sql_str.c_str());
}
#endif


#if 0
thread t1([]() {
	for (int i = 0; i < 1250; ++i)
	{
		Connection conn;
		string sql_str;
		sql_str.resize(1024);
		snprintf(&sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29, "male");
		conn.connect("127.0.0.1", 3306, "root", "18277274635a", "test");
		conn.update(sql_str.c_str());
	}
	});
thread t2([]() {
	for (int i = 0; i < 1250; ++i)
	{
		Connection conn;
		string sql_str;
		sql_str.resize(1024);
		snprintf(&sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29, "male");
		conn.connect("127.0.0.1", 3306, "root", "18277274635a", "test");
		conn.update(sql_str.c_str());
	}
	});
thread t3([]() {
	for (int i = 0; i < 1250; ++i)
	{
		Connection conn;
		string sql_str;
		sql_str.resize(1024);
		snprintf(&sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29, "male");
		conn.connect("127.0.0.1", 3306, "root", "18277274635a", "test");
		conn.update(sql_str.c_str());
	}
	});
thread t4([]() {
	for (int i = 0; i < 1250; ++i)
	{
		Connection conn;
		string sql_str;
		sql_str.resize(1024);
		snprintf(&sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29, "male");
		conn.connect("127.0.0.1", 3306, "root", "18277274635a", "test");
		conn.update(sql_str.c_str());
	}
	});

t1.join();
t2.join();
t3.join();
t4.join();
#endif