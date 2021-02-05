#include <iostream>
#include "includes/Config.hpp"

int main()
{
	try {
		Config c("../webserv.conf");
		std::cout << "config contains: " << c.getServers().size() << " servers" << std::endl;
		for (size_t i = 0; i < c.getServers().size(); i++) {
			std::cout << "server " << i + 1;
			if (!i)
				std::cout << "(default)" << std::endl;
			else
				std::cout << std::endl;
			std::cout << "\tnames: ";
			for (auto n : c.getServerNames(i))
				std::cout << n << " ";
			std::cout << std::endl;
			std::cout << "\tip: " << c.getServerIp(i) << std::endl;
			std::cout << "\tport: " << c.getServerPort(i) << std::endl;
			std::cout << "\terror pages: ";
			for (auto n : c.getServerErrorPages(i))
				std::cout << n << " ";
			std::cout << std::endl;
			std::cout << "\tmax size of body: " << c.getServerMaxBodySize(i) << " bytes" << std::endl;
			std::cout << "\tserver contains " << c.getServerById(i)._locations.size() << " locations" << std::endl;
			for (size_t j = 0; j < c.getServerById(i)._locations.size() ; j++) {
				std::cout << "\tlocation ";
				for (auto n : c.getLocationName(i, j))
					std::cout << n << " ";
				std::cout << std::endl;
				std::cout << "\t\troot: " << c.getLocationRoot(i, j) << std::endl;
				std::cout << "\t\tindex pages: ";
				for (auto n : c.getLocationIndexPages(i, j))
					std::cout << n << " ";
				std::cout << std::endl;
				std::cout << "\t\tautoindex: " << c.getLocationAutoIndex(i, j) << std::endl;
				std::cout << "\t\tallowed methods: ";
				for (auto n : c.getLocationMethods(i, j))
					std::cout << n << " ";
				std::cout << std::endl;
				std::cout << "\t\tCGI extensions: ";
				for (auto n : c.getLocationCGIExtensions(i, j))
					std::cout << n << " ";
				std::cout << std::endl;
				std::cout << "\t\tCGI path: " << c.getLocationCGIPath(i, j) << std::endl;
				std::cout << "\t\tupload storage path: " << c.getLocationUploadPath(i, j) << std::endl;
			}
		}

	}
	catch (const std::exception& ex) {
		std::cout << "exception: " << ex.what() << std::endl;
	}

}
