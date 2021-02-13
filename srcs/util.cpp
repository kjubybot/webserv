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
    len = s.length() - 1;
    while (len > pos && isspace(s[len]))
        --len;
    if (len == pos && isspace(s[pos]))
        return std::string();
    else
        ++len;
    len -= pos;
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
	int r;
	std::string result;
	char buf[BUFF_SIZE];
	while ((r = read(fd, buf, BUFF_SIZE)))
		result.append(buf, r);
	close(fd);
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
	size_t startPos = 0, endPos = str.size() - 1;

	while (startPos < str.size()) {
		if (str[startPos] == ' ' || (str[startPos] >= 9 && str[startPos] <= 13))
			startPos++;
		else
			break ;
	}
	while (endPos > 0) {
		if (str[endPos] == ' ' || (str[endPos] >= 9 && str[endPos] <= 13))
			endPos--;
		else
			break ;
	}
	return (str.substr(startPos, endPos - startPos + 1));
}

uint64_t pow(int num, size_t pow)
{
	uint64_t result = 1;
	while(pow > 0) {
		result *= num;
		pow--;
	}
	return (result);
}

std::string joinPath(const std::string& a, const std::string& b) {
    std::string ret;

    if (a[a.length() - 1] == '/' && b[0] == '/')
        ret = a.substr(0, a.length() - 1);
    else if (a[a.length() - 1] != '/' && b[0] != '/')
        ret = a + "/";
    else
        ret = a;
    return ret + b;
}

unsigned long stoul(const std::string& str, int base) {
    unsigned long ret = 0;
    std::string alpha = std::string("0123456789abcdef").substr(0, base);
    size_t i = 0;

    while (isspace(str[i]))
        ++i;
    if (alpha.find(tolower(str[i])) == std::string::npos)
        throw std::invalid_argument("no conversion");
    for (; alpha.find(tolower(str[i])) != std::string::npos; ++i)
        ret = ret * alpha.length() + alpha.find(tolower(str[i]));
    return ret;
}

std::string to_string(int val) {
    std::string ret(64, (char)0);
    std::string alpha("0123456789");
    size_t pos = ret.length();
    bool sign = val < 0;

    while (val) {
        ret[--pos] = alpha[sign ? -(val % 10) : val % 10];
        val /= 10;
    }
    if (sign)
        ret[--pos] = '-';
    return ret.substr(pos);
}

std::string to_string(unsigned val) {
    std::string ret(64, (char)0);
    std::string alpha("0123456789");
    size_t pos = ret.length();

    while (val) {
        ret[--pos] = alpha[val % 10];
        val /= 10;
    }
    return ret.substr(pos);
}