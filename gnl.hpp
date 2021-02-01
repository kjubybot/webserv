#ifndef WEBSERV_GNL_H
#define WEBSERV_GNL_H

#include <cstdlib>
#include <unistd.h>
#include <string>

#define BUFF_SIZE 4096

int get_next_line(int fd, std::string& line);

#endif
