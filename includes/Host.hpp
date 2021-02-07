#ifndef WEBSERV_HOST_HPP
#define WEBSERV_HOST_HPP

#include <arpa/inet.h>
#include <dirent.h>
#include <vector>
#include <map>
#include <string>
#include <sys/stat.h>
#include "HttpErrorPage.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"

class Host {
//	typedef struct	location {
//		std::vector<std::string> 	name;
//		std::string 				root;
//		std::vector<std::string> 	allowedMethods;
//		std::string 				autoIndex;
//		std::vector<std::string> 	index;
//		std::vector<std::string>	cgiExtensions;
//		std::string 				cgiPath;
//		std::string 				uploadPath;
//	}				Location;

    struct sockaddr_in					sockAddr;
    std::vector<std::string>			names;
	std::map<std::string, std::string>	errorPages;
	uint64_t 							maxBodySize;
	std::string 						root;
	std::vector<std::string> 			index;
	// std::vector<Location>				locations;

	std::string makeAutoindex(const std::string& path) const;
	Response makeError(const std::string& code, const std::string& message);

public:
	Host(struct sockaddr_in, const Config& config);
	~Host();
	Host(const Host& h);
	Host &operator=(const Host& h);

    struct sockaddr_in getSockAddr() const;
    const std::string& getName() const;
    const std::map<std::string, std::string>& getErrorPages() const;
	uint64_t getMaxBodySize() const;
	const std::string& getRoot() const;
	const std::vector<std::string>& getIndexPages() const;

    Response processRequest(const Request& r);
};

#endif
