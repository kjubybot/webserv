#include "Request.hpp"

// Public methods

Request::Request() : firstLine(false) {

}

void Request::setPath(const std::string &path){
	Request::path = path;
}

void Request::setHtmlPage(const std::string &htmlPage) {
	Request::htmlPage = htmlPage;
}

void Request::setMethod(const std::string &method)
{
	Request::method = method;
}

const std::string &Request::getHtmlPage() const {
	return htmlPage;
}

const std::string &Request::getPath() const {
	return path;
}

const std::string &Request::getMethod() const {
	return method;
}

void Request::parse(std::string const &line) {
	std::vector<std::string> arr = split(line, " "); // split - return vector string
	if (!firstLine) {
		Request::parseFirstLine(arr);
		firstLine = true;
	}
	else if (arr.size() != 2) {
		// split - return vector string

	}
}

void Request::addElemInMap(std::string &key, std::string &value) {
	headers.insert(std::pair<std::string, std::string>(key, value));
}

Request::~Request() {
	headers.clear();
}

// Private methods

void	Request::parseFirstLine(std::vector<std::string> const &arr) {
//	std::vector<std::string> arr; // split - return vector string
	if (arr.size() == 3) {
		if ((arr[0] != "GET") || (arr[0] != "POST") || (arr[0] != "PUT") || (arr[0] != "OPTION") || arr[2] != "HTTP/1.1")
			return ; // exception - don`t have normal method or ;
		Request::setMethod(arr[0]);
		Request::setPath(arr[1]);
		// check HTTP version
	}
	else {
		// exception - bad counts of arguments;
	}
}

//void	Request::parseFirstLine(std::string const &firstLine) {
//	std::vector<std::string> arr; // split - return vector string
//	if (arr.size() == 3) {
//		Request::setMethod(arr[0]);
//		Request::setPath(arr[1]);
//	}
//	else {
//		// exceptoin - throw ();
//	}
//}

