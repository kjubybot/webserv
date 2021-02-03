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
#include "util.hpp"

class CGI
{
private :
	std::string _cgiPath;
	std::string _cgiSource;
	// Request _request;
	// Config _config;

	CGI();
	CGI& operator= (const CGI& cgi);

	std::string executeCGI();
	char** formArgs() const;
	char** formEnvs() const;

public :
	/*
	 path - path to cgi interpreter
	 file - path to script
	 request - request object, with headers and body content
	 config - config file options with his path
	*/
	explicit CGI(const std::string& path, const std::string& source); // Request, Config
	~CGI();
	CGI(const CGI& cgi);

	// returns content of file that processed by script interpreter
	std::string processCGI();
	const std::string& getPath() const;
	const std::string& getSource() const;
};

#endif
