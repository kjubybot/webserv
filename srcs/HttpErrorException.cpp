#include "HttpErrorException.hpp"

HttpErrorException::HttpErrorException(const std::string& code, const std::string& description,  Request& request)
	: _code(code), _description(description)
{
	request.addError(code, description);
}

HttpErrorException::~HttpErrorException() throw()
{ }

HttpErrorException::HttpErrorException(const HttpErrorException& ex)
	: _code(ex._code), _description(ex._description)
{ }

const char* HttpErrorException::what() const throw()
{ return ("exception: HttpErrorException"); }

const std::string& HttpErrorException::getCode() const
{ return (this->_code); }

const std::string& HttpErrorException::getDescription() const
{ return (this->_description); }
