#include "Connection.h"
#include "MySQLConnectionPool.h"
#include <iostream>

using namespace std;

int main()
{
    /*
        压力测试：对比单线程、多线程使用/不使用连接池的性能差异
        1. 不使用连接池：每次SQL请求都创建一个新的连接
        2. 使用连接池：从连接池获取连接，用完归还
    */

    clock_t begin = clock();

    // 定义插入数据的SQL语句
    // 注意：实际测试中建议根据本地MySQL配置修改表结构和字段

    thread t1(
        []()
        {
            ConnectionPool *cp = ConnectionPool::getConnectionPool();
            for (int i = 0; i < 2500; ++i)
            {
                // 使用智能指针自动归还连接
                shared_ptr<Connection> sp = cp->getConnection();
                string sql_str;
                sql_str.resize(1024);
                snprintf(
                    &sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29,
                    "male");
                sp->update(sql_str.c_str());
            }
        });
    thread t2(
        []()
        {
            ConnectionPool *cp = ConnectionPool::getConnectionPool();
            for (int i = 0; i < 2500; ++i)
            {
                shared_ptr<Connection> sp = cp->getConnection();
                string sql_str;
                sql_str.resize(1024);
                snprintf(
                    &sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29,
                    "male");
                sp->update(sql_str.c_str());
            }
        });
    thread t3(
        []()
        {
            ConnectionPool *cp = ConnectionPool::getConnectionPool();
            for (int i = 0; i < 2500; ++i)
            {
                shared_ptr<Connection> sp = cp->getConnection();
                string sql_str;
                sql_str.resize(1024);
                snprintf(
                    &sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29,
                    "male");
                sp->update(sql_str.c_str());
            }
        });
    thread t4(
        []()
        {
            ConnectionPool *cp = ConnectionPool::getConnectionPool();
            for (int i = 0; i < 2500; ++i)
            {
                shared_ptr<Connection> sp = cp->getConnection();
                string sql_str;
                sql_str.resize(1024);
                snprintf(
                    &sql_str[0], sql_str.size(), "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 29,
                    "male");
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
