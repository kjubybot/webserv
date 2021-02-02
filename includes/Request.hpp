#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <map>
#include <vector>
#include <string>

class Request {
private:
	bool firstLine;
	std::string method;
	std::string path;
	std::string htmlPage;
	std::map<std::string, std::string> headers;

	std::vector<std::string> split(const std::string& str, const std::string& delimeter);
	std::string trim(const std::string& s);
	void parseFirstLine(std::string const &arr);
	void addElemInMap(std::string &key, std::string &value);

public:
	Request();

	virtual ~Request();
	void setPath(const std::string &path);
	void setHtmlPage(const std::string &htmlPage);

	void setMethod(const std::string &method);
	const std::string &getPath() const;
	const std::string &getMethod() const;

	const std::string &getHtmlPage() const;
	void parse(std::string const &line);

	const std::map<std::string, std::string> &getHeaders() const;

};

#endif //REQUEST_HPP