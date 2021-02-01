#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <map>

class Request {
private:
	std::string method;
	std::string path;
	std::string htmlPage;
	std::map<std::string, std::string> headers;
public:
	Request();
	virtual ~Request();

	void parse(std::string const &line) const;
	void addElemInMap(std::string &key, std::string &value);

	void setPath(const std::string &path);
	void setHtmlPage(const std::string &htmlPage);

	const std::string &getPath() const;
	const std::string &getHtmlPage() const;
};

#endif //REQUEST_HPP