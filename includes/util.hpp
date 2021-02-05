#ifndef WEBSERV_GNL_H
#define WEBSERV_GNL_H

#include <cstdlib>
#include <unistd.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

#define BUFF_SIZE 4096

int get_next_line(int fd, std::string& line);
std::string trim(const std::string& s);
std::vector<std::string> split(const std::string& str, const std::string& delimeter);
std::string iptoa(uint32_t addr);
std::string getFileContent(const std::string& filename);
char* stringDup(const std::string& str);
void freeMatrix(char** matrix);
std::string skipWS(const std::string &str);
uint64_t pow(int num, size_t pow);

#endif
