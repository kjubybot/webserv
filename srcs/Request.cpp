#include "Request.hpp"

// Public methods

Request::Request() : firstLine(false) {}

void Request::setPath(const std::string &path) {
	Request::path = path;
}

void Request::setHtmlPage(const std::string &htmlPage) {
	Request::htmlPage = htmlPage;
}

void Request::setMethod(const std::string &method) {
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

const std::map<std::string, std::string> &Request::getHeaders() const
{
	return headers;
}

void Request::parse(std::string const &line) {

	if (!firstLine) {
		Request::parseFirstLine(line);
	}
	else if (firstLine) {
		size_t colon = line.find(':');
		if ((colon != std::string::npos) && line[0] != ' ' && line[colon - 1] != ' ') {
			std::string key = std::string(line, colon);
			std::string value = std::string(trim(&line[colon + 1]));
			addElemInMap(key, value);
		}
		else if (colon != std::string::npos) {
			throw HttpErrorException("400", "Bad request");
		}
	}
	else {
		throw HttpErrorException("400", "Bad request");
	}
}

Request::~Request() {
	headers.clear();
}

// Private methods

void Request::addElemInMap(std::string &key, std::string &value) {
	headers.insert(std::pair<std::string, std::string>(key, value));
}

void	Request::parseFirstLine(std::string const &line) {
	std::vector<std::string> arr = split(line, " ");
	firstLine = true;
	if (arr.size() == 3) {
		if ((arr[0] != "GET") and (arr[0] != "POST") and (arr[0] != "PUT") and (arr[0] != "HEAD") and arr[2] != "HTTP/1.1")
			throw HttpErrorException("400", "Bad request");
		Request::setMethod(arr[0]);
		Request::setPath(arr[1]);
	}
	else {
        throw HttpErrorException("400", "Bad request");
	}
}

bool Request::isReady() const {
    return false;
}