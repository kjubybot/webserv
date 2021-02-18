#ifndef WEBSERV_HOST_HPP
#define WEBSERV_HOST_HPP

#include <arpa/inet.h>
#include <dirent.h>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <sys/stat.h>
#include <regex>
#include "HttpErrorPage.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"

class CGI;

class Host {
    typedef Config::ConfigServer::ConfigLocation conf_loc;

    struct sockaddr_in									sockAddr;
    std::vector<std::string>							names;
	std::map<std::string, std::string>					errorPages;
	uint64_t 											maxBodySize;
	std::string 										root;
	std::vector<std::string> 							index;
	uint16_t											port;
	std::vector<std::string>							auth;
	std::list<conf_loc>                                 locations;
	std::list<conf_loc>                                 regLocations;

	std::string makeAutoindex(const std::string& path) const;
	Response makeError(const std::string& code, const std::string& message, const std::string& root);
	std::list<conf_loc>::iterator matchLocation(const std::string& loc);
	bool matchExtension(const std::string& ext, conf_loc& loc);
	bool isAuthorized(const Request& request);

    static bool forSortingByLength(const conf_loc& a, const conf_loc& b);
public:
	Host(const Config::ConfigServer& server);
	~Host();
	Host(const Host& h);
	Host &operator=(const Host& h);

    struct sockaddr_in getSockAddr() const;
    std::string getName() const;
	uint64_t getMaxBodySize(const Request& request);
	const std::string& getRoot() const;
	const std::vector<std::string>& getIndexPages() const;
	uint16_t getPort() const;

    Response processRequest(const Request& r);
};

#include "CGI.hpp"

#endif
