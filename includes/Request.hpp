#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "HttpErrorException.hpp"
#include "util.hpp"

class Request {
private:
	bool firstLine;
	bool firstPart;
	int contentLen;
	int transferLen;


	std::string method;
	std::string path;
	std::string htmlPage;

	std::map<std::string, std::string> headers;

	void parseSecondPart(std::string &line);
	void parseFirstLine(std::string const &arr);
	void addElemInMap(std::string &key, std::string &value);
public:
	Request();
	virtual ~Request();

	void setPath(const std::string &path);
	void setMethod(const std::string &method);
	void setHtmlPage(const std::string &htmlPage);

	const std::string &getPath() const;
	const std::string &getMethod() const;
	const std::string &getHtmlPage() const;
	const std::map<std::string, std::string> &getHeaders() const;

	void resetRequest();
	void parse(std::string &line);

};

#endif //REQUEST_HPP