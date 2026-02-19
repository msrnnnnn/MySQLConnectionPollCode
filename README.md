# High Performance MySQL Connection Pool (基于C++11的高性能MySQL连接池)

![Language](https://img.shields.io/badge/language-C%2B%2B11-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

> **Note**: 本项目已进行全面优化，支持跨平台编译（Linux/Windows），并提供详细的性能测试报告。

## 📖 项目简介 (Introduction)

这是一个基于 **C++11** 标准开发的高性能 MySQL 数据库连接池项目。

在高并发的网络服务中，频繁地创建和销毁数据库连接会带来巨大的性能开销（TCP三次握手、MySQL认证等）。本项目通过预先创建并维护一定数量的数据库连接，并在多线程环境下安全地复用这些连接，显著提升了数据库访问的吞吐量和响应速度。

本项目采用了 **RAII机制**、**生产者-消费者模型**、**单例模式** 等现代C++编程技术，保证了连接池的高效、稳定和线程安全。

## ✨ 核心特性 (Key Features)

*   **⚡ 高性能连接复用**：
    *   避免了重复的TCP连接建立和释放，将数据库操作的延迟降至最低。
    *   使用 `std::queue` 维护空闲连接，获取和归还的时间复杂度为 O(1)。

*   **🔒 线程安全 (Thread Safety)**：
    *   基于 `std::mutex` 互斥锁和 `std::condition_variable` 条件变量，确保在多线程高并发环境下的数据一致性。
    *   实现了线程安全的连接获取与归还机制。

*   **🛡️ 智能资源管理 (RAII)**：
    *   利用 `std::shared_ptr` 管理连接生命周期，通过自定义**删除器 (Deleter)** 实现连接的自动归还。
    *   用户无需手动释放连接，彻底杜绝了连接泄露 (Connection Leak) 的风险。

*   **📈 动态扩容与缩容**：
    *   **动态扩容**：内置**生产者线程**，当连接池负载过高（空闲连接不足）时，自动检测并创建新连接（不超过最大连接数）。
    *   **闲置回收**：内置**扫描线程**，定期检测并回收长时间未使用的多余连接，释放系统资源。

*   **⚙️ 灵活配置**：
    *   支持通过 `mysql.ini` 配置文件动态调整数据库参数（IP、端口、用户）及连接池参数（初始连接数、最大连接数、超时时间等）。

## 🛠️ 技术栈 (Tech Stack)

*   **开发语言**：C++11 (使用了 `std::thread`, `std::mutex`, `std::unique_lock`, `std::condition_variable`, `std::atomic`, `std::shared_ptr`, `lambda expression` 等特性)
*   **数据库**：MySQL 5.7 / 8.0 (C API)
*   **构建工具**：CMake (跨平台支持) / Visual Studio
*   **开发环境**：Windows / Linux

## 🚀 快速开始 (Getting Started)

### 1. 环境准备

*   C++11 兼容的编译器 (GCC/Clang/MSVC)
*   CMake 3.10+
*   MySQL Server & Client Library (libmysql)

### 2. 数据库配置

在项目根目录下创建一个 `mysql.ini` 文件（可参考 `mysql.ini.example`），并填入您的数据库信息：

```ini
# MySQL Configuration
ip=127.0.0.1
port=3306
username=root
password=your_password
dbname=your_db_name
initSize=10
maxSize=1024
maxIdleTime=60
connectionTimeOut=100
```

### 3. 编译与运行

#### 使用 CMake (推荐)

```bash
mkdir build
cd build
cmake ..
# 如果找不到MySQL，请指定路径:
# cmake -DMYSQL_INCLUDE_DIR="/path/to/mysql/include" -DMYSQL_LIBRARY="/path/to/mysql/lib/libmysql.lib" ..

cmake --build .
```

#### 使用 Visual Studio

1.  打开解决方案文件 `.sln` (如果存在)。
2.  确保项目属性中包含了 MySQL 的 `include` 和 `lib` 目录。
3.  编译并运行。

## 📊 性能测试 (Benchmark)

项目中包含了一个简单的压力测试 (`main.cpp`)，对比了**单线程不使用连接池**、**多线程不使用连接池**与**多线程使用连接池**的性能差异。

| 数据量 | 未使用连接池花费时间 | 使用连接池花费时间 | 性能提升倍数 (近似) |
| :--- | :--- | :--- | :--- |
| **1,000** | 单线程: 4994ms <br> 四线程: 1578ms | 单线程: 1309ms <br> **四线程: 501ms** | **~3.1倍** |
| **5,000** | 单线程: 22735ms <br> 四线程: 6961ms | 单线程: 6277ms <br> **四线程: 2269ms** | **~3.0倍** |
| **10,000** | 单线程: 46030ms <br> 四线程: 13836ms | 单线程: 11919ms <br> **四线程: 4417ms** | **~3.1倍** |

**结论**：
1.  **连接池优势明显**：在四线程并发环境下，使用连接池的性能是未使用连接池的 **3倍以上**。
2.  **随着数据量增加，优势依然稳固**：无论是插入1000条还是10000条数据，连接池都能保持稳定的高性能表现。
3.  **并发优势**：多线程配合连接池使用，相比单线程未使用连接池，性能提升可达 **10倍以上** (46030ms vs 4417ms)。

*(注：测试环境为本地 MySQL 数据库，具体数据取决于硬件配置和数据库负载)*

## 📄 许可证 (License)

本项目采用 [MIT License](LICENSE) 许可证。
