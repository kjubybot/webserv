#ifndef HTTPERRORPAGE_HPP
#define HTTPERRORPAGE_HPP

#include <string>

class HttpErrorPage
{
private :
	std::string _code;
	std::string	_description;
	std::string _content;

	HttpErrorPage();
	HttpErrorPage(const HttpErrorPage& page);
	HttpErrorPage& operator= (const HttpErrorPage& page);

public :
	explicit HttpErrorPage(const std::string& code, const std::string& description);
	~HttpErrorPage();

	const std::string& createPage() const;
};

#endif

