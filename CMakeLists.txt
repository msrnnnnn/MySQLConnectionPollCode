cmake_minimum_required(VERSION 3.10)
project(MySQLConnectionPool VERSION 1.0.0 LANGUAGES CXX)

# Use C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Threading Library (Required for C++11 threads)
find_package(Threads REQUIRED)

# Find MySQL
# Note: On Windows, you might need to set MYSQL_DIR or manually specify include/lib paths if not found.
# Example: cmake -DMYSQL_INCLUDE_DIR="C:/Program Files/MySQL/MySQL Server 8.0/include" -DMYSQL_LIBRARY="C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.lib" ..

# Try to find MySQL automatically
find_package(MySQL QUIET)

# Manual fallback for MySQL paths
if (NOT MySQL_FOUND)
    if (WIN32)
        # Common default path on Windows
        set(MYSQL_INCLUDE_DIR "C:/Program Files/MySQL/MySQL Server 8.0/include" CACHE PATH "MySQL Include Directory")
        set(MYSQL_LIBRARY "C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.lib" CACHE FILEPATH "MySQL Library")
    else()
        # Common default path on Linux
        set(MYSQL_INCLUDE_DIR "/usr/include/mysql" CACHE PATH "MySQL Include Directory")
        set(MYSQL_LIBRARY "mysqlclient" CACHE FILEPATH "MySQL Library")
    endif()
endif()

# Add Include Directories
if (EXISTS "${MYSQL_INCLUDE_DIR}")
    include_directories("${MYSQL_INCLUDE_DIR}")
    message(STATUS "Using MySQL Include Dir: ${MYSQL_INCLUDE_DIR}")
else()
    message(WARNING "MySQL Include Directory not found! Please set MYSQL_INCLUDE_DIR.")
endif()

# Add Executable
add_executable(MySQLConnectionPool
    main.cpp
    connection.cpp
    MySQLConnectionPool.cpp
)

# Link Libraries
if (MySQL_FOUND)
    target_link_libraries(MySQLConnectionPool PRIVATE MySQL::MySQL Threads::Threads)
else()
    target_link_libraries(MySQLConnectionPool PRIVATE "${MYSQL_LIBRARY}" Threads::Threads)
endif()

# Copy config file to build directory for convenience
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mysql.ini.example ${CMAKE_CURRENT_BINARY_DIR}/mysql.ini COPYONLY)
