#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <map>

class Request {
private:
	std::string path;
	std::string htmlPage;
	std::map<std::string, std::string> headers;
public:
	Request(const std::string &htmlPage);
	virtual ~Request();

	void parse(std::string const &line) const;
	void addElemInMap(std::string &key, std::string &value);
	const std::string &getPath() const;
	const std::string &getHtmlPage() const;
};

#endif //REQUEST_HPP