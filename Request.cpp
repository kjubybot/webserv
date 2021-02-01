#include "Request.hpp"

Request::Request(const std::string &htmlPage) : htmlPage(htmlPage) {}

void Request::setPath(const std::string &path){
	Request::path = path;
}

const std::string &Request::getHtmlPage() const {
	return htmlPage;
}

const std::string &Request::getPath() const {
	return path;
}

void Request::addElemInMap(std::string &key, std::string &value) {
	headers.insert(std::pair<std::string, std::string>(key, value));
}

Request::~Request() {

}

