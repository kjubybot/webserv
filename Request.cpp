#include "Request.hpp"

Request::Request() {}

void Request::setPath(const std::string &path){
	Request::path = path;
}

const std::string &Request::getHtmlPage() const {
	return htmlPage;
}

const std::string &Request::getPath() const {
	return path;
}

void Request::parse(std::string const &line) {

}

void Request::addElemInMap(std::string &key, std::string &value) {
	headers.insert(std::pair<std::string, std::string>(key, value));
}

Request::~Request() {
	headers.clear();
}

void Request::setHtmlPage(const std::string &htmlPage) {
	Request::htmlPage = htmlPage;
}

