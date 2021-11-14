cmake_minimum_required(VERSION 3.15)
project("webserver")
set(CMAKE_CXX_STANDARD 11)
set(WEB_SERVER_SRC
        main.cpp
        webserver.h webserver.cpp
        config.h config.cpp
        CGImysql/sql_connection_pool.h CGImysql/sql_connection_pool.cpp
        http/http_conn.h http/http_conn.cpp
        lock/locker.h
        threadpool/threadpool.h
        timer/lst_timer.h timer/lst_timer.cpp timer/time_wheel.h timer/time_wheel.cpp)
add_executable(main.cpp ${WEB_SERVER_SRC})