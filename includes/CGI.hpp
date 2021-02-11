#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <string>
#include <stdexcept>
#include <map>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Config.hpp"
#include "Request.hpp"
#include "util.hpp"

class CGI
{
private :
	std::string _cgiPath;
	std::string _cgiSource;
	Request request;
	// Config		_config;

	CGI();
	CGI& operator= (const CGI& cgi);

	std::string executeCGI();
	char** formArgs() const;
	char** formEnvs() const;

public :
	explicit CGI(const std::string& path, const std::string& source, const Request& request);
	~CGI();
	CGI(const CGI& cgi);

	std::string processCGI();
	const std::string& getPath() const;
	const std::string& getSource() const;
};

#endif
