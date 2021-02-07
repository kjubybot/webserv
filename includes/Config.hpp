#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fcntl.h>
#include "util.hpp"

class Config
{
private :
	typedef struct	configServer {
		typedef struct	configLocation {
			std::vector<std::string> 	_name;
			std::string 				_root;
			std::vector<std::string> 	_allowedMethods;
			std::string 				_autoIndex;
			std::vector<std::string> 	_index;
			std::vector<std::string>	_cgiExtensions;
			std::string 				_cgiPath;
			std::string 				_uploadPath;
		}				ConfigLocation;

		std::vector<std::string>			_names;
		std::string 						_host;
		std::string							_port;
		std::string 						_root;
		std::vector<std::string> 			_index;
		std::map<std::string, std::string>	_errorPages;
		std::string							_maxBodySize;
		std::vector<ConfigLocation>			_locations;
	}				ConfigServer;

	std::string					_confPath;
	std::vector<ConfigServer>	_servers;


	Config();
	Config& operator= (const Config& config);

	void parseConfig();
	void parseServerBlock(std::vector<std::string> lines, size_t* endBlockPos);
	void parseServerBlockDirectives(std::vector<std::string> line, size_t endBlockPos);
	void parseLocationBlock(std::vector<std::string> lines, size_t* endBlockPos);
	void parseLocationBlockDirectives(std::vector<std::string> line, size_t endBlockPos);
	bool validateListen(const std::string& arg);
	bool validateIp(const std::string& ip);
	bool validateMaxBodySize(std::string size);

public :
	explicit Config(const std::string& path);
	~Config();
	Config(const Config& config);

	const std::string& getConfigPath() const;
	std::vector<ConfigServer> getServers() const;
	Config::ConfigServer getDefaultServer() const;
	std::vector<std::string> getDefaultServerNames() const;
	std::string getDefaultServerIp() const;
	uint16_t getDefaultServerPort() const;
	std::map<std::string, std::string> getDefaultServerErrorPages() const;
	uint64_t getDefaultServerMaxBodySize() const;
	std::string getDefaultServerRoot() const;
	std::vector<std::string> getDefaultServerIndexPages() const;
	std::vector<ConfigServer::ConfigLocation> getDefaultServerLocations() const;

	Config::ConfigServer getServerById(size_t idx) const;
	std::vector<std::string> getServerNames(size_t idx) const;
	std::string getServerIp(size_t idx) const;
	uint16_t getServerPort(size_t idx) const;
	std::map<std::string, std::string> getServerErrorPages(size_t idx) const;
	uint64_t getServerMaxBodySize(size_t idx) const;
	std::string getServerRoot(size_t idx) const;
	std::vector<std::string> getServerIndexPages(size_t idx) const;
	std::vector<std::string> getLocationName(size_t servIdx, size_t locIdx) const;
	std::string getLocationRoot(size_t servIdx, size_t locIdx) const;
	std::vector<std::string> getLocationMethods(size_t servIdx, size_t locIdx) const;
	bool getLocationAutoIndex(size_t servIdx, size_t locIdx) const;
	std::vector<std::string> getLocationIndexPages(size_t servIdx, size_t locIdx) const;
	std::vector<std::string> getLocationCGIExtensions(size_t servIdx, size_t locIdx) const;
	std::string getLocationCGIPath(size_t servIdx, size_t locIdx) const;
	std::string getLocationUploadPath(size_t servIdx, size_t locIdx) const;
};

#endif
