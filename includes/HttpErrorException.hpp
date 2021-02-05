#ifndef HTTPERROREXCEPTION_HPP
#define HTTPERROREXCEPTION_HPP

#include <stdexcept>
#include <string>

class Request;

class HttpErrorException : public std::exception
{
private :
	std::string _code;
	std::string _description;

	HttpErrorException();
	HttpErrorException& operator= (const HttpErrorException& ex);

public :
	explicit HttpErrorException(const std::string& code, const std::string& description, Request& request);
	virtual ~HttpErrorException() throw();
	HttpErrorException(const HttpErrorException& ex);

	const char* what() const throw();
	const std::string& getCode() const;
	const std::string& getDescription() const;
};

#include "Request.hpp"

#endif
