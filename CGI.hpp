#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <map>
#include <unistd.h>
#include <sys/types.h>

class CGI
{
private :
	std::string _cgiPath;
	std::string _cgiFile;

	CGI();
	CGI& operator= (const CGI& cgi);
	void processContent() const;
	char** getEnvs() const;

public :
	/*
	 path - path to cgi interpreter
	 file - path to script
	 request - request object, with headers and body content
	 config - config file options with his path
	*/
	explicit CGI(const std::string& path, const std::string& file); // Request, Config
	~CGI();
	CGI(const CGI& cgi);

	// returns content of file that processed by script interpreter
	void getProcessedContent() const;

};

#endif
