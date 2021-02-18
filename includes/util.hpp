#ifndef WEBSERV_GNL_H
#define WEBSERV_GNL_H

#include <cstdlib>
#include <unistd.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>

#define BUFF_SIZE (1 << 19)

int get_next_line(int fd, std::string& line);
std::string trim(const std::string& s);
std::vector<std::string> split(const std::string& str, const std::string& delimeter);
std::string iptoa(uint32_t addr);
std::string getFileContent(const std::string& filename);
char* stringDup(const std::string& str);
void freeMatrix(char** matrix);
std::string skipWS(const std::string &str);
uint64_t pow(int num, size_t pow);
std::string joinPath(const std::string& a, const std::string& b);
unsigned long stoul(const std::string& str, int base = 10);
std::string to_string(int val);
std::string to_string(unsigned val);
std::string encodeBase64(const std::string& input);
std::string decodeBase64(const std::string& input);

template <class C>
bool isIn(C& container, typename C::value_type val) {
    for (typename C::iterator it = container.begin(); it != container.end(); ++it)
        if (val == *it)
            return true;
    return false;
}

#endif
