#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <arpa/inet.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "HttpErrorException.hpp"
#include "util.hpp"

class Request {
	bool firstPart;
	bool secondPart;
	bool flagError;
	uint64_t contentLen;
	struct sockaddr_in sockAddr;
	size_t toRead;

	std::string path;
	std::string method;
	std::string content;

	std::map<std::string, std::string> headers;
	std::pair<std::string, std::string> error;

    void parseFirst(std::string &line);
    void parseSecond(std::string& line);
    int checkSymbols(char sym);
public:

    Request(struct sockaddr_in sockAddr);
    virtual ~Request();

	void setPath(const std::string &path);
    void setMethod(const std::string &method);

    uint64_t getContentLen() const;
	const std::string &getPath() const;
    const std::string &getMethod() const;
    const std::string &getContent() const;
    std::pair<std::string, std::string> getError() const;
    const std::map<std::string, std::string> &getHeaders() const;

    void parse(std::string& line);

    bool isFirstPart() const;
    bool isFlagError() const;
    bool isSecondPart() const;

    void addError(std::string errorKey, std::string errorValue);

    const sockaddr_in& getSockAddr() const;
};

#endif
