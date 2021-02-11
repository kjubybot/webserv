#include <iostream>
#include "includes/Config.hpp"

int main()
{
	try {
		Config conf("../webserv.conf");

		std::cout << "found " << conf.getServers().size() << " servers config"
				  << std::endl;
		std::vector<Config::ConfigServer> servers = conf.getServers();
		size_t pos = 1;
		for (std::vector<Config::ConfigServer>::iterator it = servers.begin();
			 it != servers.end(); it++) {
			std::cout << "server " << pos;
			if (pos == 1)
				std::cout << " - default server" << std::endl;
			else
				std::cout << std::endl;

			std::cout << "\tnames: ";
			std::vector<std::string> names = it->getNames();
			for (std::vector<std::string>::iterator itN = names.begin();
				 itN != names.end(); itN++) {
				std::cout << *itN << " ";
			}
			std::cout << std::endl;

			std::cout << "\thost: " << it->getHost() << std::endl;
			std::cout << "\tport: " << it->getPort() << std::endl;
			std::cout << "\troot: " << it->getRoot() << std::endl;

			std::cout << "\tindex pages: ";
			std::vector<std::string> ip = it->getIndexPages();
			for (std::vector<std::string>::iterator itN = ip.begin();
				 itN != ip.end(); itN++) {
				std::cout << *itN << " ";
			}
			std::cout << std::endl;

			std::cout << "\terror pages: " << std::endl;
			std::map<std::string, std::string> ep = it->getErrorPages();
			for (std::map<std::string, std::string>::iterator itN = ep.begin();
				 itN != ep.end(); itN++) {
				std::cout << "\t\t" << itN->first << " => " << itN->second
						  << std::endl;
			}

			std::cout << "\tmax body size: " << it->getMaxBodySize()
					  << std::endl;

			std::list<Config::ConfigServer::ConfigLocation> locs = it->getLocations();
			std::cout << "\tserver has " << locs.size() << " locations"
					  << std::endl;

			for (std::list<Config::ConfigServer::ConfigLocation>::iterator itt = locs.begin();
				 itt != locs.end(); itt++) {
				std::cout << "\t\tname: " << itt->getName() << std::endl;
				std::cout << "\t\troot: " << itt->getRoot() << std::endl;

				std::cout << "\t\tmethods: ";
				std::vector<std::string> m = itt->getMethods();
				for (std::vector<std::string>::iterator itN = m.begin();
					 itN != m.end(); itN++) {
					std::cout << *itN << " ";
				}
				std::cout << std::endl;

				std::cout << std::boolalpha << "\t\tautoindex: "
						  << itt->getAutoIndex() << std::endl;

				std::cout << "\t\tindex pages: ";
				std::vector<std::string> ipp = itt->getIndexPages();
				for (std::vector<std::string>::iterator itN = ipp.begin();
					 itN != ipp.end(); itN++) {
					std::cout << *itN << " ";
				}
				std::cout << std::endl;

				std::cout << "\t\tCGI extensions: ";
				std::vector<std::string> ext = itt->getCGIExtensions();
				for (std::vector<std::string>::iterator itN = ext.begin();
					 itN != ext.end(); itN++) {
					std::cout << *itN << " ";
				}
				std::cout << std::endl;

				std::cout << "\t\tCGI path: " << itt->getCGIPath() << std::endl;
				std::cout << "\t\tupload path: " << itt->getUploadPath()
						  << std::endl;
				std::cout << "\t\tmax body size: " << itt->getMaxBodySize()
						  << std::endl;
				std::cout << std::endl;
			}

			pos++;
			std::cout << std::endl;
		}
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	return (0);
}
