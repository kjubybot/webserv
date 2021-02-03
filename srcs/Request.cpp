#include "Request.hpp"

// Public methods

Request::Request() : firstLine(false), contentLen(0) {}

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

const std::map<std::string, std::string> &Request::getHeaders() const {
	return headers;
}

#define PRINT(x) { std::cout << x << std::endl; };

void Request::parse(std::string &line) {
	size_t newLine = line.find('\n');
	std::string headLine = line;
	std::string copyLine = line.substr(0, newLine);
	headLine.substr(newLine + 1);
	while (newLine != std::string::npos) //!headLine.empty()
	{
		if (!firstLine) {
			Request::parseFirstLine(copyLine);
		}
		else if (firstLine) {
			size_t colon = copyLine.find(':');
			if ((colon != std::string::npos) && (copyLine[0] != ' ') && (copyLine[colon - 1] != ' ')) {
				std::string key = copyLine.substr(0, colon);
				std::string value = std::string(trim(&copyLine[colon + 1]));
				std::transform(key.begin(), key.end(), key.begin(), ::tolower);
				if (key == "content-length") {
					contentLen = std::stoi(value);
				}
				if (contentLen && method == "GET") {
					throw HttpErrorException("400", "Bad request");
				}
				addElemInMap(key, value);
			}
			else if (colon != std::string::npos) {
				throw HttpErrorException("400", "Bad request");
			}
		}
		else {
			throw HttpErrorException("400", "Bad request");
		}
		newLine = headLine.find('\n');
		copyLine = headLine.substr(0, newLine);
		headLine.erase(0, newLine + 1);
		PRINT("Asd");
	}
	std::map<std::string, std::string>::iterator it;

	for (it= this->headers.begin(); it != headers.end(); ++it)
	{
		PRINT("|" << it->first << "|" << ": " << it->second)
	}
}

void Request::resetRequest() {
	headers.clear();
}

Request::~Request() {
	headers.clear();
}

// Private methods

void Request::addElemInMap(std::string &key, std::string &value) {

	if (headers.count(key) == 1)
		throw HttpErrorException("400", "Bad request");
	else
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
