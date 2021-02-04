#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <fcntl.h>
#include "util.hpp"

class Config
{
private :
	class ConfigServer
	{
	friend class Config;
	private :
		class ConfigLocation
		{
		friend class Config;
		friend class ConfigServer;
		private :
			std::vector<std::string> 	_name;
			std::string 				_root;
			std::vector<std::string> 	_allowedMethods;
			bool 						_autoIndex;
			std::vector<std::string> 	_index;
			std::vector<std::string>	_extensions;
			std::string 				_cgiPath;
			bool 						_upload;
			std::string 				_uploadPath;

//			ConfigLocation();
//			ConfigLocation(const ConfigLocation& location);
//			ConfigLocation& operator= (const ConfigLocation& location);

		public :
//			explicit ConfigLocation(...);
//			~ConfigLocation();

		};

		std::vector<std::string>	_names;
		std::string 				_host;
		size_t 						_port;
		std::vector<std::string>	_errorPages;
		uint64_t 					_maxBodySize;
		std::vector<ConfigLocation>	_locations;

//		ConfigServer();
//		ConfigServer(const ConfigServer& server);
//		ConfigServer& operator= (const ConfigServer& server);

	public :
//		explicit ConfigServer(...);
//		~ConfigServer();

	};

	std::string					_confPath;
	std::vector<ConfigServer>	_servers;


	Config();
	Config(const Config& config);
	Config& operator= (const Config& config);

	void parseConfig();
	void parseServerDirective(std::vector<std::string> line);
	bool validateListen(const std::string& arg);
	bool validateIp(const std::string& ip);
	bool validateMaxBodySize(std::string size);

public :
	explicit Config(const std::string path);
	~Config();


};

#endif
