#pragma once
#include <queue>
#include <mutex>
#include "connection.h"
#include <string>
#include <atomic>
#include <memory>
#include <condition_variable>
//实现的是MySql连接池功能
class ConnectionPool
{
public:
	//获取连接池对象实例
	static ConnectionPool* getConnectionPool();
	//给外界提供获取连接池空闲连接的接口
	shared_ptr<Connection> getConnection();
private:
	ConnectionPool();
	ConnectionPool(const ConnectionPool&) = delete;
	ConnectionPool& operator=(const ConnectionPool&) = delete;

	//从配置文件中加载配置项
	bool loadConfigFile();

	//在新线程中，负责生产新连接
	void produceConnectionTask();

	//回收连接池中超过最大空闲时间的连接
	void recycleConnection();

	string _ip;//MySQL的ip地址
	unsigned short _port;//MySQL的端口号
	string _username;//MySQL用户名
	string _password;//MySQL密码
	string _dbname;//MySQL数据库名称
	int _initSize;//连接池初始连接量
	int _maxSize;//连接池最大连接量
	int _maxIdleTime;//连接池中空闲连接时间
	int _connectionTimeOut;//连接池获取连接的超时时间

	std::queue<Connection*> _ConnectionPoolQueue;//连接池连接队列
	std::mutex _queueMutex;//维护队列线程安全的互斥锁
	atomic_int _connectionCount;//记录创建连接的总数量
	std::condition_variable cv;
};