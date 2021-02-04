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
	bool finishBody;
	bool flagError;
	size_t remainder;
	int contentLen;
	size_t transferLen;

	std::string path;
	std::string method;
	std::string content;
	std::string htmlPage;

	std::map<std::string, std::string> headers;
	std::map<std::string, std::string> error;

	void addError(std::string errorKey, std::string errorValue);
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

	void parse(std::string &line);

	bool isFinishBody() const;

	bool isFlagError() const;

    const std::pair<std::string, std::string> getError() const;

};

#endif //REQUEST_HPP