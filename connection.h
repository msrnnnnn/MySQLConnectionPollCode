#pragma once
#include "public.h"
#include <ctime>
#include <mysql.h>
#include <string>

/*
 * @File: connection.h
 * @Brief: MySQL连接的封装声明
 */

class Connection
{
public:
    // 初始化数据库连接
    Connection();

    // 释放数据库连接资源
    ~Connection();

    // 禁用拷贝构造和拷贝赋值，避免浅拷贝导致MYSQL指针重复释放
    Connection(const Connection &) = delete;
    Connection &operator=(const Connection &) = delete;

    // 连接数据库
    bool connect(std::string ip, unsigned short port, std::string username, std::string password, std::string dbname);

    // 更新操作 insert、delete、update
    bool update(std::string sql);

    // 查询操作 select
    MYSQL_RES *query(std::string sql);

    // 刷新当前空闲时间点
    void refreshTime()
    {
        _idelTime = clock();
    }

    // 获取连接空闲时长
    clock_t getIdelTime()
    {
        return clock() - _idelTime;
    }

private:
    MYSQL *_conn;      // 表示和MySQL Server的一条连接
    clock_t _idelTime; // 记录空闲时间
};
