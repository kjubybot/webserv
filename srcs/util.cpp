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
    while (pos < s.length() && isspace(s[pos]))
        ++pos;
    if (pos == s.length())
        return std::string();
    len = 0;
    while (len + pos < s.length() && !isspace(s[len + pos]))
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

std::string skipWS(const std::string &str)
{
	std::string result(str);
	result.erase(0, result.find_first_not_of(' '));
	result.erase(result.find_last_not_of(' ') + 1);
	result.erase(0, result.find_first_not_of('\t'));
	result.erase(result.find_last_not_of('\t') + 1);
	return (result);
}
