#ifndef HTTPERRORPAGE_HPP
#define HTTPERRORPAGE_HPP

#include <string>
#include <fcntl.h>
#include <unistd.h>

class HttpErrorPage
{
private :
	std::string _code;
	std::string	_description;
	std::string _content;
	int 		_fd;

	HttpErrorPage();
	HttpErrorPage(const HttpErrorPage& page);
	HttpErrorPage& operator= (const HttpErrorPage& page);

public :
	explicit HttpErrorPage(const std::string& code, const std::string& description);
	~HttpErrorPage();

	std::string createPage();
};

#endif
