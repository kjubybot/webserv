#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Request.hpp"
#include "util.hpp"

class Host;

class CGI
{
private :
	std::string _cgiPath;
	std::string _cgiSource;
	Request 	_request;

	CGI();
	CGI& operator= (const CGI& cgi);

	void executeCGI(const Host& host);
	char** formArgs() const;
	char** formEnvs(const Host& host) const;

public :
	explicit CGI(const std::string& path, const std::string& source, const Request& request);
	~CGI();
	CGI(const CGI& cgi);

	std::string processCGI(const Host& host);
	const std::string& getPath() const;
	const std::string& getSource() const;
};

#include "Host.hpp"

#endif
