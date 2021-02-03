#include "Request.hpp"

//#define PRINT(x) { std::cout << x << std::endl; };
// Public methods

Request::Request() : firstLine(false), firstPart(false), contentLen(0) {}

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

void Request::parse(std::string &line) {
	if (!firstPart)
	{
		size_t newLine = line.find('\n');
		std::string copyLine = line.substr(0, newLine);
		line.erase(0, newLine + 1);
		if (copyLine.empty() && headers.count("host") == 1)
		{
			std::cout << "End parsing 1st part" << std::endl; // parse second part
			firstPart = true;
			return;
		}
		while (newLine != std::string::npos)
		{
			if (!firstLine) {
				Request::parseFirstLine(copyLine);
			}
			else if (firstLine)
			{
				size_t colon = copyLine.find(':');
				if ((colon != std::string::npos) && (copyLine[0] != ' ') && (copyLine[colon - 1] != ' '))
				{
					std::string key = copyLine.substr(0, colon);
					copyLine.erase(0, colon + 1);
					copyLine.erase(0, copyLine.find_first_not_of(' '));
					copyLine.erase(copyLine.find_last_not_of(' ') + 1);
					std::string value = copyLine;
					std::transform(key.begin(), key.end(), key.begin(), ::tolower);
					if (key == "content-length")
					{
						contentLen = std::stoi(value);
					}
					if (contentLen && method == "GET")
					{
						throw HttpErrorException("400", "Bad request");
					}
					addElemInMap(key, value);
				}
				else if (colon != std::string::npos)
				{
					throw HttpErrorException("400", "Bad request");
				}
			}
			else
			{
				throw HttpErrorException("400", "Bad request");
			}
			newLine = line.find('\n');
			copyLine = line.substr(0, newLine);
			line.erase(0, newLine);
//		PRINT("Asd");
		}
//	std::map<std::string, std::string>::iterator it;
//
//	for (it= this->headers.begin(); it != headers.end(); ++it) {
//		PRINT("|" << it->first << "|" << ": " << it->second)
//	}
	}
	else {
		parseSecondPart(line);
	}

}

void Request::resetRequest() {
	headers.clear();
}

Request::~Request() {
	headers.clear();
}

// Private methods

void Request::parseSecondPart(std::string &line) {
	// if (GET)
		// return;

	// if : Content-Len && POST - need limit size read
	// if : Transfer-Encoding and == chunked, read len symbols
	if (headers.count("transfer-encoding") == 1 && headers["transfer-encoding"] == "chunked") {

	}
	else {
		throw HttpErrorException("501", "Not Implemented");
	}




}

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
