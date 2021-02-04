#include "Config.hpp"

Config::Config(const std::string path)
	: _confPath(path)
{
	parseConfig();

	std::cout << _servers[0]._maxBodySize << " " << _servers[0]._host << " " << _servers[0]._port << std::endl;
	for (auto n : _servers[0]._names)
		std::cout << n << std::endl;
	for (auto n : _servers[0]._errorPages)
		std::cout << n << std::endl;

	std::cout << _servers.size() << std::endl;
 }

Config::~Config()
{ }

void Config::parseConfig()
{
	int fd = open(this->_confPath.c_str(), O_RDONLY);
	if (fd < 0)
		throw std::runtime_error("can't open file: " + this->_confPath);

	std::vector<std::string> lines;
	std::string tmp;
	while (get_next_line(fd, tmp)) {
		if (!skipWS(tmp).empty())
			lines.push_back(tmp);
	}

	for (size_t i = 0; i < lines.size(); i++) {
		std::vector<std::string> lineItems = split(lines[i], " ");

		size_t endBlockPos = i + 1;
		if ((lineItems[0] == "server" &&
			 lineItems[lineItems.size() - 1] == "{" && lineItems.size() == 2) ||
			(lineItems[0] == "server{" && lineItems.size() == 1)) {
			int bracketsDiff = 1;
			this->_servers.push_back(ConfigServer());

			while (endBlockPos < lines.size()) {
				lineItems = split(lines[endBlockPos], " ");
				if (lineItems[lineItems.size() - 1] == "{" &&
					lineItems.size() > 2)
					bracketsDiff++;
				else if (std::find(lineItems.begin(), lineItems.end(), "{") !=
						 lineItems.end()) {
					this->_servers.clear();
					throw std::runtime_error("error occurred in config");
				}
				else if (lineItems[lineItems.size() - 1] == "}" &&
						 lineItems.size() == 1) {
					bracketsDiff--;
					if (bracketsDiff == 0)
						break;
				}
				else if (std::find(lineItems.begin(), lineItems.end(), "}") !=
						   lineItems.end()) {
					this->_servers.clear();
					throw std::runtime_error("error occurred in config");
				}
				else
					parseServerDirective(lineItems);
				endBlockPos++;
			}
			i = endBlockPos;
		}
		else
			throw std::runtime_error("error occurred in config");
	}
}

void Config::parseServerDirective(std::vector<std::string> line)
{
	std::vector<std::string> directives;
	std::string directive = line[0];
	int directiveStatus;

	if (directive == "location")
		directiveStatus = 2;
	else {
		directives.push_back("server_name");
		directives.push_back("listen");
		directives.push_back("error_page");
		directives.push_back("max_body_size");
		std::vector<std::string>::iterator it;
		for (it = directives.begin(); it != directives.end(); it++) {
			if (directive == *it) {
				if (line.size() < 2)
					directiveStatus = 0;
				else
					directiveStatus = 1;
				break ;
			}
		}
		if (it == directives.end())
			directiveStatus = 0;
	}

	if (directiveStatus == 0) {
		this->_servers.clear();
		throw std::runtime_error("error occurred in config");
	}
	else if (directiveStatus == 1) {
		if (line[0] == "server_name") {
			if (this->_servers.back()._names.empty())
				this->_servers.back()._names.insert(this->_servers.back()._names.begin(),
					++line.begin(), line.end());
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config");
			}
		}
		else if (line[0] == "listen") {
			if (line.size() != 2 || !validateListen(line[1])) {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config");
			}
			else {
				this->_servers.back()._host = line[1].substr(0, line[1].find(":"));
				this->_servers.back()._port = std::stoi(line[1].substr(line[1].find(":") + 1));
			}
		}
		else if (line[0] == "error_page") {
			if (this->_servers.back()._errorPages.empty())
				this->_servers.back()._errorPages.insert(this->_servers.back()._errorPages.begin(),
					++line.begin(), line.end());
			else {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config");
			}
		}
		else {
			if (line.size() != 2 || !validateMaxBodySize(line[1])) {
				this->_servers.clear();
				throw std::runtime_error("error occurred in config");
			}
			else {
				std::map<std::string, size_t> exponentAccord;
				exponentAccord[""] = 1;
				exponentAccord["K"] = pow(2, 10) * exponentAccord[""];
				exponentAccord["M"] = pow(2, 10) * exponentAccord["K"];
				exponentAccord["G"] = pow(2, 10) * exponentAccord["M"];
				int num = std::stoi(line[1].substr(0, line[1].size() - 1));
				this->_servers.back()._maxBodySize = (uint64_t)num *
													 exponentAccord[line[1].substr(line[1].size() - 1)];
			}
		}
	}
	else {
		// parsing location clock
	}
}

bool Config::validateListen(const std::string& arg)
{
	if (arg.find(':') == std::string::npos ||
		arg.find_first_of(':') != arg.find_last_of(':'))
		return (false);

	std::string ip = arg.substr(0, arg.find_first_of(':'));
	std::string port =  arg.substr(arg.find_first_of(':') + 1);
	if (!validateIp(ip))
		return (false);

	int intPort;
	try {
		intPort = std::stoi(port);
	}
	catch (const std::exception& ex) {
		return (false);
	}
	if (intPort > UINT16_MAX || intPort < 0)
		return (false);
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

// string format num + ''/K/M/G  : 12M =
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
	std::string exponent = size.substr(digitsEndPos + 1);

	int intNumber;
	try {
		intNumber = std::stoi(number);
	}
	catch (const std::exception& ex) {
		std::cerr << "exception: " << ex.what() << std::endl;
		return (false);
	}
	if (intNumber < 0)
		return (false);
	if (exponent.size() == 0 || (exponent.size() == 1 &&
		(exponent == "K", exponent == "M", exponent == "G")))
		return (true);
	return (false);
}
