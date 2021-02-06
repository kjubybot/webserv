#include "Config.hpp"

Config::Config(const std::string& path)
	: _confPath(path)
{
	parseConfig();
	if (this->_servers.empty())
		throw std::runtime_error("error occurred in config: empty config");
}

Config::~Config()
{ }

Config::Config(const Config& config)
	: _confPath(config._confPath), _servers(config._servers)
{ }

void Config::parseConfig()
{
	int fd = open(this->_confPath.c_str(), O_RDONLY);
	if (fd < 0)
		throw std::runtime_error("can't open config: " + this->_confPath);

	std::vector<std::string> lines;
	std::string tmp;
	while (get_next_line(fd, tmp))
		lines.push_back(tmp);
	if (!skipWS(tmp).empty())
		lines.push_back(tmp);

	for (size_t i = 0; i < lines.size(); i++) {
		if (skipWS(lines[i]).empty())
			continue ;

		std::vector<std::string> lineItems = split(lines[i], " ");
		size_t endBlockPos = i + 1;
		if ((lineItems[0] == "server" && lineItems[1] == "{" && lineItems.size() == 2) ||
			(lineItems[0] == "server{" && lineItems.size() == 1)) {
			parseServerBlock(lines, &endBlockPos);
			i = endBlockPos;
		}
		else
			throw std::runtime_error("error occurred in config: line " + std::to_string(i + 1));
	}
}

void Config::parseServerBlock(std::vector<std::string> lines, size_t* endBlockPos)
{
	this->_servers.push_back(ConfigServer());
	bool bracketOpen = true;

	while (*endBlockPos < lines.size()) {
		if (skipWS(lines[*endBlockPos]).empty()) {
			(*endBlockPos)++;
			continue ;
		}

		std::vector<std::string> lineItems = split(lines[*endBlockPos], " ");
		if (lineItems[0] == "location" && lineItems[lineItems.size() - 1] == "{" && lineItems.size() > 2)
			parseLocationBlock(lines, endBlockPos);
		else if (std::find(lineItems.begin(), lineItems.end(), "{") != lineItems.end()) {
			this->_servers.clear();
			throw std::runtime_error("error occurred in config: line " + std::to_string(*endBlockPos + 1));
		}
		else if (lineItems[0] == "}" && lineItems.size() == 1) {
			bracketOpen = false;
			break ;
		}
		else if (std::find(lineItems.begin(), lineItems.end(), "}") != lineItems.end()) {
			this->_servers.clear();
			throw std::runtime_error("error occurred in config: line " + std::to_string(*endBlockPos + 1));
		}
		else
			parseServerBlockDirectives(lineItems, *endBlockPos);
		(*endBlockPos)++;
	}

	if (bracketOpen) {
		this->_servers.clear();
		throw std::runtime_error("error occurred in config: line " + std::to_string(*endBlockPos + 1));
	}
	else {
		if (this->_servers.back()._host == "") {
			this->_servers.clear();
			throw std::runtime_error("error occurred in config: line " + std::to_string(*endBlockPos + 1));
		}
		else if (this->_servers.back()._root == "")
			this->_servers.back()._root = ".";
	}
}

void Config::parseServerBlockDirectives(std::vector<std::string> line, size_t endBlockPos)
{
	static std::vector<std::string> serverBlockDirectives;
	serverBlockDirectives.push_back("server_name"); serverBlockDirectives.push_back("listen");
	serverBlockDirectives.push_back("error_page"); serverBlockDirectives.push_back("max_body_size");
	serverBlockDirectives.push_back("root"); serverBlockDirectives.push_back("index");
	bool isValidDirective = false;

	for(std::vector<std::string>::iterator it = serverBlockDirectives.begin(); it != serverBlockDirectives.end(); it++) {
		if (line[0] == *it) {
			isValidDirective = true;
			break ;
		}
	}

	if (!isValidDirective) {
		this->_servers.clear();
		throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
	}
	else {
		if (line[0] == "server_name") {
			if (this->_servers.back()._names.empty())
				this->_servers.back()._names.insert(this->_servers.back()._names.begin(),
					++line.begin(), line.end());
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else if (line[0] == "listen") {
			if (line.size() == 2 && validateListen(line[1]) && this->_servers.back()._host == "") {
				this->_servers.back()._host = line[1].substr(0, line[1].find(":"));
				this->_servers.back()._port = line[1].substr(line[1].find(":") + 1);
			}
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else if (line[0] == "error_page") {
			if (line.size() == 3 && this->_servers.back()._errorPages.count(line[1]) == 0)
				this->_servers.back()._errorPages[line[1]] = line[2];
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else if (line[0] == "max_body_size") {
			if (this->_servers.back()._maxBodySize == "" && line.size() == 2 && validateMaxBodySize(line[1]))
				this->_servers.back()._maxBodySize = line[1];
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else if (line[0] == "root") {
			if (line.size() == 2 && this->_servers.back()._root == "")
				this->_servers.back()._root = line[1];
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else {
			if (this->_servers.back()._index.empty())
				this->_servers.back()._index.insert(this->_servers.back()._index.begin(), ++line.begin(), line.end());
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
	}
}

void Config::parseLocationBlock(std::vector<std::string> lines, size_t* endBlockPos)
{
	this->_servers.back()._locations.push_back(ConfigServer::ConfigLocation());
	std::vector<std::string> locationLine = split(lines[*endBlockPos], " ");
	for (size_t i = 1; i < locationLine.size() - 1; i++)
		this->_servers.back()._locations.back()._name.push_back(locationLine[i]);
	(*endBlockPos)++;
	bool bracketOpen = true;
	while (*endBlockPos < lines.size()) {
		if (skipWS(lines[*endBlockPos]).empty()) {
			(*endBlockPos)++;
			continue ;
		}
		std::vector<std::string> lineItems = split(lines[*endBlockPos], " ");

		if (std::find(lineItems.begin(), lineItems.end(), "{") != lineItems.end()) {
			this->_servers.clear();
			throw std::runtime_error("error occurred in config: line " + std::to_string(*endBlockPos + 1));
		}
		else if (lineItems[0] == "}" && lineItems.size() == 1) {
			bracketOpen = false;
			break ;
		}
		else if (std::find(lineItems.begin(), lineItems.end(), "}") != lineItems.end()) {
			this->_servers.clear();
			throw std::runtime_error("error occurred in config: line " + std::to_string(*endBlockPos + 1));
		}
		else
			parseLocationBlockDirectives(lineItems, *endBlockPos);
		(*endBlockPos)++;
	}

	if (bracketOpen)  {
		this->_servers.clear();
		throw std::runtime_error("error occurred in config: line " + std::to_string(*endBlockPos + 1));
	}
}

void Config::parseLocationBlockDirectives(std::vector<std::string> line, size_t endBlockPos)
{
	static std::vector<std::string> locationBlockDirectives;
	locationBlockDirectives.push_back("root"); locationBlockDirectives.push_back("index");
	locationBlockDirectives.push_back("autoindex"); locationBlockDirectives.push_back("method");
	locationBlockDirectives.push_back("cgi_extensions"); locationBlockDirectives.push_back("cgi_path");
	locationBlockDirectives.push_back("upload_storage");
	bool isValidDirective = false;

	std::vector<std::string>::iterator it = locationBlockDirectives.begin();
	for( ; it != locationBlockDirectives.end(); it++) {
		if (line[0] == *it) {
			isValidDirective = true;
			break ;
		}
	}

	if (!isValidDirective) {
		this->_servers.clear();
		throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
	}
	else {
		if (line[0] == "root") {
			if (this->_servers.back()._locations.back()._root == "" && line.size() == 2)
				this->_servers.back()._locations.back()._root = line[1];
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else if (line[0] == "index") {
			if (this->_servers.back()._locations.back()._index.empty())
				this->_servers.back()._locations.back()._index.insert(
				this->_servers.back()._locations.back()._index.begin(),
						++line.begin(), line.end()
				);
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else if (line[0] == "autoindex") {
			if (this->_servers.back()._locations.back()._autoIndex == "" &&
				line.size() == 2 && (line[1] == "on" || line[1] == "off"))
				this->_servers.back()._locations.back()._autoIndex = line[1];
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else if (line[0] == "method") {
			if (this->_servers.back()._locations.back()._allowedMethods.empty())
				this->_servers.back()._locations.back()._allowedMethods.insert(
				this->_servers.back()._locations.back()._allowedMethods.begin(),
						++line.begin(), line.end()
				);
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else if (line[0] == "cgi_extensions") {
			if (this->_servers.back()._locations.back()._cgiExtensions.empty())
				this->_servers.back()._locations.back()._cgiExtensions.insert(
						this->_servers.back()._locations.back()._cgiExtensions.begin(),
						++line.begin(), line.end()
				);
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else if (line[0] == "cgi_path") {
			if (this->_servers.back()._locations.back()._cgiPath == "" && line.size() == 2)
				this->_servers.back()._locations.back()._cgiPath = line[1];
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else {
			if (this->_servers.back()._locations.back()._uploadPath == "" && line.size() == 2)
				this->_servers.back()._locations.back()._uploadPath = line[1];
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
	}
}

bool Config::validateListen(const std::string& arg)
{
	if (arg.find(':') == std::string::npos || arg.find_first_of(':') != arg.find_last_of(':'))
		return (false);

	std::string ip = arg.substr(0, arg.find_first_of(':'));
	std::string port =  arg.substr(arg.find_first_of(':') + 1);
	if (!validateIp(ip))
		return (false);
	int intPort;
	try {
		intPort = std::stoi(port);
		if (intPort > UINT16_MAX || intPort < 0)
			return (false);
	}
	catch (const std::exception& ex) {
		return (false);
	}
	return (true);
}

bool Config::validateIp(const std::string& ip)
{
	std::vector<std::string> octets = split(ip, ".");

	if (octets.size() != 4)
		return (false);
	try {
		for (std::vector<std::string>::iterator it = octets.begin(); it != octets.end(); it++) {
			int octet = std::stoi(*it);
			if (octet > UINT8_MAX || octet < 0)
				return (false);
		}
	}
	catch (const std::exception& ex) {
		std::cerr << "exception: " << ex.what() << std::endl;
		return (false);
	}
	return (true);
}

bool Config::validateMaxBodySize(std::string size)
{
	size_t digitsEndPos = 0;
	while (digitsEndPos < size.size()) {
		if (std::isdigit(size[digitsEndPos]))
			digitsEndPos++;
		else
			break ;
	}

	std::string number = size.substr(0, digitsEndPos);
	std::string exponent = size.substr(digitsEndPos);
	int intNumber;
	try {
		intNumber = std::stoi(number);
		if (intNumber < 0)
			return (false);
	}
	catch (const std::exception& ex) {
		std::cerr << "exception: " << ex.what() << std::endl;
		return (false);
	}
	if (exponent.size() == 0 || (exponent.size() == 1 && (exponent == "K" || exponent == "M" || exponent == "G")))
		return (true);
	return (false);
}

Config::ConfigServer Config::getDefaultServer() const
{ return (this->_servers.front()); }

std::vector<std::string> Config::getDefaultServerNames() const
{ return (this->_servers.front()._names); }

std::string Config::getDefaultServerIp() const
{ return (this->_servers.front()._host); }

uint16_t Config::getDefaultServerPort() const
{ return ((uint16_t)std::stoi(this->_servers.front()._port)); }

std::map<std::string, std::string> Config::getDefaultServerErrorPages() const
{ return (this->_servers.front()._errorPages); }

uint64_t Config::getDefaultServerMaxBodySize() const
{ return (getServerMaxBodySize(0)); }

std::string Config::getDefaultServerRoot() const
{ return (this->_servers.front()._root); }

std::vector<std::string> Config::getDefaultServerIndexPages() const
{ return (this->_servers.front()._index); }

std::vector<Config::ConfigServer::ConfigLocation> Config::getDefaultServerLocations() const
{ return (this->_servers.front()._locations); }

Config::ConfigServer Config::getServerById(size_t idx) const
{ return (this->_servers.at(idx)); }

const std::string& Config::getConfigPath() const
{ return (this->_confPath); }

std::vector<Config::ConfigServer> Config::getServers() const
{ return (this->_servers); }

std::vector<std::string> Config::getServerNames(size_t idx) const
{ return (this->_servers.at(idx)._names); }

std::string Config::getServerIp(size_t idx) const
{ return (this->_servers.at(idx)._host); }

uint16_t Config::getServerPort(size_t idx) const
{ return ((uint16_t)std::stoi(this->_servers.at(idx)._port)); }

std::map<std::string, std::string> Config::getServerErrorPages(size_t idx) const
{ return (this->_servers.at(idx)._errorPages); }

uint64_t Config::getServerMaxBodySize(size_t idx) const
{
	std::map<std::string, size_t> exponentAccord;
	exponentAccord[""] = 1;
	exponentAccord["K"] = pow(2, 10) * exponentAccord[""];
	exponentAccord["M"] = pow(2, 10) * exponentAccord["K"];
	exponentAccord["G"] = pow(2, 10) * exponentAccord["M"];
	if (!(this->_servers.at(idx)._maxBodySize.empty())) {
		int num = std::stoi(this->_servers.at(idx)._maxBodySize.substr(0,
		this->_servers.at( idx)._maxBodySize.size() - 1));
		uint64_t result = (uint64_t) num *
			exponentAccord[this->_servers.at(idx)._maxBodySize.substr(this->_servers.at(idx)._maxBodySize.size() - 1)];
		return (result);
	}
	else
		return (UINT64_MAX);
}

std::string Config::getServerRoot(size_t idx) const
{ return (this->_servers.at(idx)._root); }

std::vector<std::string> Config::getServerIndexPages(size_t idx) const
{ return (this->_servers.at(idx)._index); }

std::vector<std::string> Config::getLocationName(size_t servIdx, size_t locIdx) const
{ return (this->_servers.at(servIdx)._locations.at(locIdx)._name); }

std::string Config::getLocationRoot(size_t servIdx, size_t locIdx) const
{ return (this->_servers.at(servIdx)._locations.at(locIdx)._root); }

std::vector<std::string> Config::getLocationMethods(size_t servIdx, size_t locIdx) const
{ return (this->_servers.at(servIdx)._locations.at(locIdx)._allowedMethods); }

bool Config::getLocationAutoIndex(size_t servIdx, size_t locIdx) const
{ return (this->_servers.at(servIdx)._locations.at(locIdx)._autoIndex == "on" ? true : false); }

std::vector<std::string> Config::getLocationIndexPages(size_t servIdx, size_t locIdx) const
{ return (this->_servers.at(servIdx)._locations.at(locIdx)._index); }

std::vector<std::string> Config::getLocationCGIExtensions(size_t servIdx, size_t locIdx) const
{ return (this->_servers.at(servIdx)._locations.at(locIdx)._cgiExtensions); }

std::string Config::getLocationCGIPath(size_t servIdx, size_t locIdx) const
{ return (this->_servers.at(servIdx)._locations.at(locIdx)._cgiPath); }

std::string Config::getLocationUploadPath(size_t servIdx, size_t locIdx) const
{ return (this->_servers.at(servIdx)._locations.at(locIdx)._uploadPath); }
