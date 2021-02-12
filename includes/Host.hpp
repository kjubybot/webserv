#ifndef WEBSERV_HOST_HPP
#define WEBSERV_HOST_HPP

#include <arpa/inet.h>
#include <dirent.h>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <sys/stat.h>
#include "HttpErrorPage.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "CGI.hpp"

class Host {
    typedef Config::ConfigServer::ConfigLocation conf_loc;
    struct sockaddr_in									sockAddr;
    std::vector<std::string>							names;
	std::map<std::string, std::string>					errorPages;
	uint64_t 											maxBodySize;
	std::string 										root;
	std::vector<std::string> 							index;
	std::list<conf_loc>	locations;

	std::string makeAutoindex(const std::string& path) const;
	Response makeError(const std::string& code, const std::string& message, const std::string& root);
	std::list<conf_loc>::iterator matchLocation(const std::string& loc);

    static bool forSortingByLength(const conf_loc& a, const conf_loc& b);
public:
	Host(const Config::ConfigServer& server);
	~Host();
	Host(const Host& h);
	Host &operator=(const Host& h);

    struct sockaddr_in getSockAddr() const;
    std::string getName() const;
    const std::map<std::string, std::string>& getErrorPages() const;
	uint64_t getMaxBodySize(const Request& request);
	const std::string& getRoot() const;
	const std::vector<std::string>& getIndexPages() const;

    Response processRequest(const Request& r);
};

#endif
