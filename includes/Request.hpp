#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "HttpErrorException.hpp"
#include "util.hpp"

class Request {
private:
	bool firstPart;
	bool secondPart;
	bool flagError;
	int contentLen;

	std::string path;
	std::string method;
	std::string content;
	std::string htmlPage;

	std::map<std::string, std::string> headers;
	std::pair<std::string, std::string> error;

    void parseFirst(std::string &line);
    void parseSecond(std::string& line);
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
    void parse(std::string& line);


    bool isFirstPart() const;

    bool isFlagError() const;

    const std::pair<std::string, std::string> getError() const;

    void addError(std::string errorKey, std::string errorValue);

    bool isSecondPart() const;

};

#endif //REQUEST_HPP
