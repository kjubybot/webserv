#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <fcntl.h>
#include "util.hpp"

class Config
{
public :
	typedef struct	configServer {
		typedef struct	configLocation {
			std::string				 	_name;
			std::string 				_root;
			std::vector<std::string> 	_allowedMethods;
			std::string 				_autoIndex;
			std::vector<std::string> 	_index;
			std::vector<std::string>	_cgiExtensions;
			std::string 				_cgiPath;
			std::string 				_uploadPath;
			uint64_t					_maxBodySize;

			std::string getName() const;
			std::string getRoot() const;
			std::vector<std::string> getMethods() const;
			bool getAutoIndex() const;
			std::vector<std::string> getIndexPages() const;
			std::vector<std::string> getCGIExtensions()const;
			std::string getCGIPath() const;
			std::string getUploadPath() const;
			uint64_t getMaxBodySize() const;
		}				ConfigLocation;

		std::vector<std::string>			_names;
		std::string 						_host;
		std::string							_port;
		std::string 						_root;
		std::vector<std::string> 			_index;
		std::map<std::string, std::string>	_errorPages;
		std::string							_maxBodySize;
		std::list<ConfigLocation>			_locations;

		std::vector<std::string> getNames() const;
		std::string getHost() const;
		uint16_t getPort() const;
		std::string getRoot() const;
		std::vector<std::string> getIndexPages() const;
		std::map<std::string, std::string> getErrorPages() const;
		uint64_t getMaxBodySize() const;
		std::list<ConfigLocation> getLocations() const;
	}				ConfigServer;

private :
	std::string					_confPath;
	std::vector<ConfigServer>	_servers;

	Config();
	Config& operator= (const Config& config);

	void parseConfig();
	void parseServerBlock(std::vector<std::string> lines, size_t* endBlockPos);
	void parseServerBlockDirectives(std::vector<std::string> line, size_t endBlockPos);
	void parseLocationBlock(std::vector<std::string> lines, size_t* endBlockPos);
	void parseLocationBlockDirectives(std::vector<std::string> line, size_t endBlockPos);
	void fillLocationsDefault();
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
	std::list<ConfigServer::ConfigLocation> getDefaultServerLocations() const;
	Config::ConfigServer getServerById(size_t idx) const;
	std::queue<ConfigServer> getServersQueue() const;
};

#endif
