cmake_minimum_required(VERSION 3.17)
project(webserv)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_COMPILER clang++)

add_compile_options(-Wall -Wextra -Werror)
include_directories(includes)

add_executable(webserv srcs/main.cpp
        srcs/Host.cpp
        srcs/Connection.cpp
        srcs/Server.cpp
        srcs/util.cpp
        srcs/Request.cpp
        srcs/Response.cpp
        srcs/HttpErrorPage.cpp
        srcs/HttpErrorException.cpp
        srcs/CGI.cpp
        srcs/Config.cpp)
