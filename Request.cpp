#include "Request.hpp"

// Public methods

Request::Request() : firstLine(false) {

}

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

void Request::parse(std::string const &line) {

	if (!firstLine) {
		Request::parseFirstLine(line);
	}
	else if (firstLine) {
		int colon = line.find(':');
		if ((colon != std::string::npos) && (line.find(' ', colon) == std::string::npos)) {
			// need check space before ':' - and and
			addElemInMap(std::string(&line[0], 5), std::string(&line[colon + 1]));
		}
		else {
			// exception! - "400" "Bad request"
		}




	}
//	else if (arr.size() == 2 || arr.size() == 3) {
		// split - return vector string
//		if (arr[0][arr[0].length() - 1] == ':' && arr[1][0] == ':') {
//			 exception! - "400" "Bad request"
//		}
//		addElemInMap(arr[0], arr.size() == 2 ? arr[1] : arr[2]);
//	}
	else {
		// exception! - "400" "Bad request"
	}
}

void Request::addElemInMap(std::string &key, std::string &value) {
	headers.insert(std::pair<std::string, std::string>(key, value));
}

Request::~Request() {
	headers.clear();
}

// Host: :   google.com
// asd asd asd asdas asdasd

// Private methods

void	Request::parseFirstLine(std::string const &line) {
	std::vector<std::string> arr = split(line, " ");
	firstLine = true;
	if (arr.size() == 3) {
		if ((arr[0] != "GET") || (arr[0] != "POST") || (arr[0] != "PUT") || (arr[0] != "OPTION") || arr[2] != "HTTP/1.1")
			return ; // exception - don`t have normal method or ;
		Request::setMethod(arr[0]);
		Request::setPath(arr[1]);
	}
	else {
		// exception - bad counts of arguments;
	}
}

std::vector<std::string> split(const std::string& str, const std::string& delimeter) {
	std::vector<std::string> items;
	size_t prev = 0, pos = str.find(delimeter, prev);

	while ((pos < str.length()) && (prev < str.length())) {
		if (pos == std::string::npos)
			pos = str.length();
		std::string item = str.substr(prev, pos - prev);
		if (!item.empty())
			items.push_back(item);
		prev = pos + delimeter.length();
		pos = str.find(delimeter, prev);
	}
	if (str.size() != prev)
		items.push_back(str.substr(prev, pos - prev));
	return (items);
}
