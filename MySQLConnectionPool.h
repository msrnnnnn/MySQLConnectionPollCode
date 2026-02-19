#pragma once
#include "connection.h"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

using namespace std;

/*
 * 实现单例模式的MySQL连接池管理
 */
class ConnectionPool
{
public:
    // 获取连接池对象实例
    static ConnectionPool *getConnectionPool();
    // 给外部提供获取连接池中连接的接口
    shared_ptr<Connection> getConnection();

private:
    // 单例模式构造函数私有化
    ConnectionPool();
    ConnectionPool(const ConnectionPool &) = delete;
    ConnectionPool &operator=(const ConnectionPool &) = delete;

    // 加载配置文件
    bool loadConfigFile();

    // 运行在独立的线程中，负责生产新连接
    void produceConnectionTask();

    // 扫描超过maxIdleTime时间的空闲连接，进行回收
    void recycleConnection();

    string _ip;             // MySQL的ip地址
    unsigned short _port;   // MySQL的端口号
    string _username;       // MySQL用户名
    string _password;       // MySQL密码
    string _dbname;         // MySQL数据库名称
    int _initSize;          // 连接池的初始连接量
    int _maxSize;           // 连接池的最大连接量
    int _maxIdleTime;       // 连接池最大空闲时间
    int _connectionTimeOut; // 连接池获取连接的超时时间

    std::queue<Connection *> _ConnectionPoolQueue; // 存储mysql连接的队列
    std::mutex _queueMutex;                        // 维护连接队列的线程安全互斥锁
    atomic_int _connectionCount;                   // 记录连接所创建的connection连接的总数量
    std::condition_variable cv;                    // 设置条件变量，用于连接生产线程和消费线程的通信
};
