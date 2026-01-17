#pragma once
#include "public.h"
#include <mysql.h>
#include <string>
#include <ctime>
using namespace std;
//实现连接数据库的操作

class Connection
{
public:
    // 初始化数据库连接
    Connection();

    // 释放数据库连接资源
    ~Connection();

    // 连接数据库
    bool connect(string ip, unsigned short port, string username,string password,string dbname);

    // 更新操作 insert、delete、update
    bool update(string sql);

    // 查询操作 select
    MYSQL_RES* query(string sql);

    //刷新当前时间
    void refreshTime() { _idelTime = clock(); };

    //得到空闲时间
    time_t getIdelTime() { return clock() - _idelTime; };
private:
    MYSQL* _conn; // 表示和MySQL Server的一条连接
    time_t _idelTime;
};