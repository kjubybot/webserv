#include <list>
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
	while (get_next_line(fd, tmp)) {
		if (tmp.find_first_of('#') != std::string::npos)
			tmp = tmp.substr(0, tmp.find_first_of('#'));
		lines.push_back(tmp);
	}

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
		if (this->_servers.back()._root == "")
			this->_servers.back()._root = ".";
		fillLocationsDefault();
		fillServerNames();
	}
}

void Config::parseServerBlockDirectives(std::vector<std::string> line, size_t endBlockPos)
{
	static std::vector<std::string> serverBlockDirectives;
	serverBlockDirectives.push_back("server_name"); serverBlockDirectives.push_back("listen");
	serverBlockDirectives.push_back("error_page"); serverBlockDirectives.push_back("max_body_size");
	serverBlockDirectives.push_back("root"); serverBlockDirectives.push_back("index");
	serverBlockDirectives.push_back("usr");
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
			if (line.size() == 2 && validateListen(line[1]) && this->_servers.back()._host.empty()) {
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
			if (this->_servers.back()._maxBodySize.empty() && line.size() == 2 && validateMaxBodySize(line[1]))
				this->_servers.back()._maxBodySize = line[1];
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else if (line[0] == "root") {
			if (line.size() == 2 && this->_servers.back()._root.empty())
				this->_servers.back()._root = line[1];
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else if (line[0] == "index") {
			if (this->_servers.back()._index.empty())
				this->_servers.back()._index.insert(this->_servers.back()._index.begin(), ++line.begin(), line.end());
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else {
			if (line.size() == 2) {
				this->_servers.back()._auth.push_back(line[1]);
			}
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
		this->_servers.back()._locations.back()._name = locationLine[1];
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
	locationBlockDirectives.push_back("upload_storage"); locationBlockDirectives.push_back("max_body_size");
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
			if (this->_servers.back()._locations.back()._root.empty() && line.size() == 2)
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
			if (this->_servers.back()._locations.back()._autoIndex.empty() &&
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
			if (this->_servers.back()._locations.back()._cgiPath.empty() && line.size() == 2)
				this->_servers.back()._locations.back()._cgiPath = line[1];
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else if (line[0] == "max_body_size") {
			if (this->_servers.back()._locations.back()._maxBodySize.empty() && line.size() == 2  && validateMaxBodySize(line[1]))
				this->_servers.back()._locations.back()._maxBodySize = line[1];
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
		else {
			if (this->_servers.back()._locations.back()._uploadPath.empty() && line.size() == 2)
				this->_servers.back()._locations.back()._uploadPath = line[1];
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: line " + std::to_string(endBlockPos + 1));
			}
		}
	}
}

void Config::fillLocationsDefault()
{
	std::list<ConfigServer::ConfigLocation>::iterator it;
	for (it = this->_servers.back()._locations.begin(); it != this->_servers.back()._locations.end(); it++) {
		if (it->_root.empty()) {
			it->_root = this->_servers.back()._root;
		}
		if (it->_index.size() == 0) {
			it->_index = this->_servers.back()._index;
		}
		if (it->_maxBodySize.empty()) {
			it->_maxBodySize = this->_servers.back()._maxBodySize;
		}
	}
	
	it = this->_servers.back()._locations.begin();
	std::vector<std::list<Config::ConfigServer::ConfigLocation>::iterator> regLocs;
	for ( ; it != this->_servers.back()._locations.end(); it++) {
		if (it->getName()[it->getName().length() - 1] == '$')
			regLocs.push_back(it);
	}

	for (size_t i = 0; i < regLocs.size(); i++) {
		this->_servers.back()._regexLocations.push_back(*(regLocs[i]));
		this->_servers.back()._locations.erase(regLocs[i]);
	}
}

void Config::fillServerNames()
{
	if (this->_servers.back().getNames().empty())
		this->_servers.back()._names.push_back(this->_servers.back()._host);

	for (std::vector<std::string>::iterator it = this->_servers.back()._names.begin();
		it != this->_servers.back()._names.end(); it++)
		*it += ":" + std::to_string(this->_servers.back().getPort());

	std::vector<std::string> names = this->_servers.back()._names;
	std::vector<ConfigServer> servs = this->_servers;
	if (servs.size() == 1)
		return ;
	for (std::vector<ConfigServer>::iterator it = servs.begin(); it != --servs.end(); it++) {
		for (std::vector<std::string>::iterator jt = names.begin(); jt != names.end(); jt++) {
			if (isIn(it->_names, *jt)) {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config: host + port is not unique");
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
{ return (this->_servers.front().getPort()); }

std::map<std::string, std::string> Config::getDefaultServerErrorPages() const
{ return (this->_servers.front()._errorPages); }

uint64_t Config::getDefaultServerMaxBodySize() const
{ return (this->_servers.front().getMaxBodySize()); }

std::string Config::getDefaultServerRoot() const
{ return (this->_servers.front()._root); }

std::vector<std::string> Config::getDefaultServerIndexPages() const
{ return (this->_servers.front()._index); }

std::vector<std::string> Config::getDefaultServerAuth() const
{ return (this->_servers.front()._auth); }

std::list<Config::ConfigServer::ConfigLocation> Config::getDefaultServerLocations() const
{ return (this->_servers.front()._locations); }

std::list<Config::ConfigServer::ConfigLocation> Config::getDefaultServerRegexLocations() const
{ return (this->_servers.front()._regexLocations); }

Config::ConfigServer Config::getServerById(size_t idx) const
{ return (this->_servers.at(idx)); }

const std::string& Config::getConfigPath() const
{ return (this->_confPath); }

std::vector<Config::ConfigServer> Config::getServers() const
{ return (this->_servers); }

std::vector<std::string> Config::ConfigServer::getNames() const
{ return (this->_names); }

std::string Config::ConfigServer::getHost() const
{ return (this->_host); }

uint16_t Config::ConfigServer::getPort() const
{ return ((uint16_t)std::stoi(this->_port)); }

std::string Config::ConfigServer::getRoot() const
{ return (this->_root); }

std::vector<std::string> Config::ConfigServer::getIndexPages() const
{ return (this->_index); }

std::map<std::string, std::string> Config::ConfigServer::getErrorPages() const
{ return (this->_errorPages); }

uint64_t Config::ConfigServer::getMaxBodySize() const
{
	std::map<std::string, size_t> exponentAccord;
	exponentAccord[""] = 1;
	exponentAccord["K"] = pow(2, 10) * exponentAccord[""];
	exponentAccord["M"] = pow(2, 10) * exponentAccord["K"];
	exponentAccord["G"] = pow(2, 10) * exponentAccord["M"];
	if (!(this->_maxBodySize.empty())) {
		uint64_t result;
		std::string exponent = this->_maxBodySize.substr(this->_maxBodySize.size() - 1);
		if (exponent == "K" || exponent == "M" || exponent == "G") {
			int num = std::stoi(this->_maxBodySize.substr(0, this->_maxBodySize.size() - 1));
			result = (uint64_t) num * exponentAccord[this->_maxBodySize.substr(this->_maxBodySize.size() - 1)];
		}
		else
			result = std::stoi(this->_maxBodySize);
		return (result);
	}
	else
		return (0);
}

std::vector<std::string> Config::ConfigServer::getAuth() const
{ return (this->_auth); }

std::list<Config::ConfigServer::ConfigLocation> Config::ConfigServer::getLocations() const
{ return (this->_locations); }

std::list<Config::ConfigServer::ConfigLocation> Config::ConfigServer::getRegexLocations() const
{ return (this->_regexLocations); }

std::string Config::ConfigServer::ConfigLocation::getName() const
{ return (this->_name); }

std::string Config::ConfigServer::ConfigLocation::getRoot() const
{ return (this->_root); }

std::vector<std::string> Config::ConfigServer::ConfigLocation::getMethods() const
{ return (this->_allowedMethods); }

bool Config::ConfigServer::ConfigLocation::getAutoIndex() const
{ return (this->_autoIndex == "on" ? true : false); }

std::vector<std::string> Config::ConfigServer::ConfigLocation::getIndexPages() const
{ return (this->_index); }

std::vector<std::string> Config::ConfigServer::ConfigLocation::getCGIExtensions()const
{ return (this->_cgiExtensions); }

std::string Config::ConfigServer::ConfigLocation::getCGIPath() const
{ return (this->_cgiPath); }

std::string Config::ConfigServer::ConfigLocation::getUploadPath() const
{ return (this->_uploadPath); }

uint64_t Config::ConfigServer::ConfigLocation::getMaxBodySize() const
{
	std::map<std::string, size_t> exponentAccord;
	exponentAccord[""] = 1;
	exponentAccord["K"] = pow(2, 10) * exponentAccord[""];
	exponentAccord["M"] = pow(2, 10) * exponentAccord["K"];
	exponentAccord["G"] = pow(2, 10) * exponentAccord["M"];
	if (!(this->_maxBodySize.empty())) {
		uint64_t result;
		std::string exponent = this->_maxBodySize.substr(this->_maxBodySize.size() - 1);
		if (exponent == "K" || exponent == "M" || exponent == "G") {
			int num = std::stoi(this->_maxBodySize.substr(0, this->_maxBodySize.size() - 1));
			result = (uint64_t) num * exponentAccord[this->_maxBodySize.substr(this->_maxBodySize.size() - 1)];
		}
		else
			result = std::stoi(this->_maxBodySize);
		return (result);
	}
	else
		return (0);
}

std::queue<Config::ConfigServer> Config::getServersQueue() const
{
	std::queue<ConfigServer> queue;
	for (std::vector<ConfigServer>::const_iterator it = this->_servers.begin(); it != this->_servers.end(); it++) {
		queue.push(*it);
	}
	return (queue);
}
