#include "MySQLConnectionPool.h"
#include "public.h"
#include "connection.h"
#include <thread>
#include <functional>
#include <chrono>
#include <memory>

#include <fstream>   // 用C++的ifstream替代C的fopen，更安全
#include <sstream>
#include <algorithm> // 用于去除空格
#include <cctype>    // isspace

//获取连接池对象实例
ConnectionPool* ConnectionPool::getConnectionPool()
{
    static ConnectionPool pool;
    return &pool;
}

// 辅助函数：去除字符串首尾空格/制表符（必须）
static string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
}
// ConnectionPool类的成员函数：加载MySQL.ini配置文件
bool ConnectionPool::loadConfigFile() {
    // 1. 打开MySQL.ini配置文件（当前程序运行目录下）
    ifstream configFile("MySQL.ini");
    if (!configFile.is_open()) {
        cerr << "[错误] 无法打开配置文件 MySQL.ini，请检查文件是否存在！" << endl;
        return false;
    }

    string line;
    // 2. 逐行读取并解析配置
    while (getline(configFile, line)) {
        line = trim(line); // 去除当前行首尾空格

        // 跳过空行和注释行（#开头）
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // 3. 分割key和value（按=分隔）
        size_t equalPos = line.find('=');
        if (equalPos == string::npos) {
            cerr << "[错误] 配置行格式错误：" << line << "（缺少=分隔符）" << endl;
            configFile.close();
            return false;
        }

        // 提取key和value并去除空格
        string key = trim(line.substr(0, equalPos));
        string value = trim(line.substr(equalPos + 1));

        // 4. 解析配置项到成员变量
        try {
            if (key == "ip") _ip = value;
            else if (key == "port") _port = stoi(value);
            else if (key == "username") _username = value;
            else if (key == "password") _password = value;
            else if (key == "dbname") _dbname = value;
            else if (key == "initSize") _initSize = stoi(value);
            else if (key == "maxSize") _maxSize = stoi(value);
            else if (key == "maxIdleTime") _maxIdleTime = stoi(value);
            else if (key == "connectionTimeOut") _connectionTimeOut = stoi(value);
            else cerr << "[警告] 未知配置项：" << key << "=" << value << endl;
        }
        catch (const invalid_argument&) {
            cerr << "[错误] 配置项 " << key << " 的值 " << value << " 不是有效整数！" << endl;
            configFile.close();
            return false;
        }
        catch (const out_of_range&) {
            cerr << "[错误] 配置项 " << key << " 的值 " << value << " 超出整数范围！" << endl;
            configFile.close();
            return false;
        }
    }

    // 5. 关闭文件并校验核心配置
    configFile.close();
    if (_ip.empty() || _username.empty() || _password.empty() || _dbname.empty()) {
        cerr << "[错误] 核心配置项（ip/username/password/dbname）不能为空！" << endl;
        return false;
    }

    // 替换原来的打印代码，改为输出所有配置项
    cout << "[成功] MySQL.ini 配置加载完成：" << endl;
    cout << "ip: " << _ip << endl;
    cout << "port: " << _port << endl;
    cout << "username: " << _username << endl;
    cout << "password: " << _password << endl;
    cout << "dbname: " << _dbname << endl;
    cout << "initSize: " << _initSize << endl;
    cout << "maxSize: " << _maxSize << endl;
    cout << "maxIdleTime: " << _maxIdleTime << "秒" << endl;
    cout << "connectionTimeOut: " << _connectionTimeOut << "毫秒" << endl;
    return true;
}

//连接池构造函数
ConnectionPool::ConnectionPool() : _connectionCount(0)
{
    //加载配置文件
    if (!loadConfigFile())
    {
        return;
    }

    //创建初始数量的连接
    for (int i = 0; i < _initSize; ++i)
    {
        Connection* conn = new Connection();
        if (!conn->connect(_ip, _port, _username, _password, _dbname))
        {
            LOG("创建连接失败");
            continue;
        }
        _ConnectionPoolQueue.push(conn);
        conn->refreshTime();
        _connectionCount++;
    }
    //创建生产者线程
    thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();
}

//在新线程中，负责生产新连接
void ConnectionPool::produceConnectionTask()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(_queueMutex);

        while (!_ConnectionPoolQueue.empty())
        {
            cv.wait(lock);
        }

        if (_connectionCount <= _maxSize)
        {
            Connection* p = new Connection();
            if (!p->connect(_ip, _port, _username, _password, _dbname))
            {
                delete p;
                LOG("生产者线程无法生产连接");
                break;
            }
            _ConnectionPoolQueue.push(p);
            p->refreshTime();
            _connectionCount++;
        }

        //通知消费者线程可以消费了
        cv.notify_all();
    }
}

//给外界提供获取连接池空闲连接的接口,消费者线程
shared_ptr<Connection> ConnectionPool::getConnection()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        while (_ConnectionPoolQueue.empty())
        {
            if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeOut)))
            {
                if (_ConnectionPoolQueue.empty())
                {
                    LOG("请求连接超时.....");
                    return nullptr;
                }
            }
        }
        //目的是不要让客户端使用完连接后直接析构，让其放回连接池
        std::shared_ptr<Connection> sp(_ConnectionPoolQueue.front(), [this](Connection* pcon) {
            //必须加锁，此时锁已经被释放了，要重新获取锁
            std::unique_lock<std::mutex> lock(_queueMutex);
            _ConnectionPoolQueue.push(pcon);
            pcon->refreshTime();
            });

        _ConnectionPoolQueue.pop();
        cv.notify_all();

        return sp;
    }
}

//回收连接池中超过最大空闲时间的连接
void ConnectionPool::recycleConnection()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(_maxIdleTime));

        std::unique_lock<std::mutex> lock(_queueMutex);
        while (_connectionCount > _initSize)
        {
            Connection* p = _ConnectionPoolQueue.front();
            if (p->getIdelTime() >= _maxIdleTime * 1000)
            {
                _ConnectionPoolQueue.pop();
                _connectionCount--;
                delete p;
            }
            else
            {
                break;
            }
        }
    }
}

