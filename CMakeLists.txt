cmake_minimum_required(VERSION 3.10)
project(MySQLConnectionPool VERSION 1.0.0 LANGUAGES CXX)

# 使用 C++11 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找线程库 (C++11 线程需要)
find_package(Threads REQUIRED)

# 查找 MySQL
# 注意：在 Windows 上，如果找不到 MySQL，可能需要设置 MYSQL_DIR 或手动指定 include/lib 路径。
# 示例：cmake -DMYSQL_INCLUDE_DIR="C:/Program Files/MySQL/MySQL Server 8.0/include" -DMYSQL_LIBRARY="C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.lib" ..

# 尝试自动查找 MySQL
find_package(MySQL QUIET)

# MySQL 路径的手动回退配置
if (NOT MySQL_FOUND)
    if (WIN32)
        # Windows 上的常见默认路径
        set(MYSQL_INCLUDE_DIR "C:/Program Files/MySQL/MySQL Server 8.0/include" CACHE PATH "MySQL Include Directory")
        set(MYSQL_LIBRARY "C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.lib" CACHE FILEPATH "MySQL Library")
    else()
        # Linux 上的常见默认路径
        set(MYSQL_INCLUDE_DIR "/usr/include/mysql" CACHE PATH "MySQL Include Directory")
        set(MYSQL_LIBRARY "mysqlclient" CACHE FILEPATH "MySQL Library")
    endif()
endif()

# 添加包含目录
if (EXISTS "${MYSQL_INCLUDE_DIR}")
    include_directories("${MYSQL_INCLUDE_DIR}")
    message(STATUS "Using MySQL Include Dir: ${MYSQL_INCLUDE_DIR}")
else()
    message(WARNING "MySQL Include Directory not found! Please set MYSQL_INCLUDE_DIR.")
endif()

# 添加可执行文件
add_executable(MySQLConnectionPool
    main.cpp
    connection.cpp
    MySQLConnectionPool.cpp
)

# 链接库
if (MySQL_FOUND)
    target_link_libraries(MySQLConnectionPool PRIVATE MySQL::MySQL Threads::Threads)
else()
    target_link_libraries(MySQLConnectionPool PRIVATE "${MYSQL_LIBRARY}" Threads::Threads)
endif()

# 为了方便，将配置文件复制到构建目录
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mysql.ini.example ${CMAKE_CURRENT_BINARY_DIR}/mysql.ini COPYONLY)
