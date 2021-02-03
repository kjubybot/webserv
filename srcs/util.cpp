#include "util.hpp"

int get_next_line(int fd, std::string& line) {
    static std::string rem;
    char buf[BUFF_SIZE];
    int r = 1;
    size_t newline;

    while ((newline = rem.find('\n')) == std::string::npos && (r = read(fd, buf, BUFF_SIZE)))
        rem.append(buf, r);
    if (newline != std::string::npos) {
        line = rem.substr(0, newline);
        rem = rem.substr(newline + 1);
        return 1;
    }
    line = rem.substr(0);
    rem.clear();
    return 0;
}

std::string trim(const std::string& s) {
    size_t pos, len;
    pos = 0;
    while (pos < s.length() && (s[pos] == '\n' || (s[pos] >= 9 && s[pos] <= 13)))
        ++pos;
    if (pos == s.length())
        return std::string();
    len = pos + 1;
    while (len < s.length() && (s[len] != '\n' && !(s[len] >= 9 && s[len] <= 13)))
        ++len;
    return s.substr(pos, len);
}

std::vector<std::string> split(const std::string& str, const std::string& delimeter) {
    std::vector<std::string> items;
    size_t prev = 0, pos = str.find(delimeter, prev);

    while ((pos < str.length()) && (prev < str.length())) {
        if (pos == std::string::npos)
            pos = str.length();
        std::string item = str.substr(prev, pos - prev);
        if (!item.empty())
            items.push_back(item);
        prev = pos + delimeter.length();
        pos = str.find(delimeter, prev);
    }
    if (str.size() != prev)
        items.push_back(str.substr(prev, pos - prev));
    return (items);
}

std::string iptoa(uint32_t addr) {
    std::string ret;
    ret.append(std::to_string(addr & 0xff));
    ret.append(".");
    ret.append(std::to_string((addr >> 8) & 0xff));
    ret.append(".");
    ret.append(std::to_string((addr >> 16) & 0xff));
    ret.append(".");
    ret.append(std::to_string(addr >> 24));
    return ret;
}

std::string getBadResponse() {
	std::string baseResponse;
	baseResponse += "HTTP/1.1 400 Bad hueta\r\n";
	baseResponse += "Content-Length: 10\r\n";
	baseResponse += "Content-Type: text/html\r\n\r\n";
	baseResponse += "Bad hueta";
	return (baseResponse);
}

std::string getBaseResponse() {
	std::string baseResponse;
	baseResponse += "HTTP/1.1 200 OK\r\n";
	baseResponse += "Content-Length: 12\r\n";
	baseResponse += "Content-Type: text/html\r\n\r\n";
	baseResponse += "Hello world";
	return (baseResponse);
}

std::string getFileContent(const std::string& filename)
{
	int fd = open(filename.c_str(), O_RDONLY);
	std::string result;
	std::string tmp;
	while (get_next_line(fd, tmp))
		result += tmp + "\n";
	if (tmp != "")
		result += tmp;
	return (result);
}

char* stringDup(const std::string& str)
{
	char* result = new char[str.size() + 1];
	size_t i = -1;
	while (str[++i] != '\0')
		result[i] = str[i];
	result[i] = '\0';
	return (result);
}

void freeMatrix(char** matrix)
{
	if (!matrix)
		return ;
	size_t i = -1;
	while (matrix[++i] != NULL)
		delete matrix[i];
	delete matrix;
}
