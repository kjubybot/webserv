#include <iostream>
#include "includes/Config.hpp"

int main()
{
	try {
		Config c("../base.conf");
		std::cout << "config contains: " << c.getServers().size() << " servers" << std::endl;
		for (size_t i = 0; i < c.getServers().size(); i++) {
			std::cout << "server " << i + 1;
			if (!i)
				std::cout << "(default)" << std::endl;
			else
				std::cout << std::endl;
			std::cout << "\tnames: ";
			for (size_t k = 0; k < c.getServerNames(i).size(); k++)
				std::cout << c.getServerNames(i)[k] << " ";
			std::cout << std::endl;
			std::cout << "\tip: " << c.getServerIp(i) << std::endl;
			std::cout << "\tport: " << c.getServerPort(i) << std::endl;
			std::cout << "\terror pages: ";
			for (size_t k = 0; k < c.getServerErrorPages(i).size(); k++)
				std::cout << c.getServerErrorPages(i)[k] << " ";
			std::cout << std::endl;
			std::cout << "\tmax size of body: " << c.getServerMaxBodySize(i) << " bytes" << std::endl;
			std::cout << "\tserver contains " << c.getServerById(i)._locations.size() << " locations" << std::endl;
			for (size_t j = 0; j < c.getServerById(i)._locations.size() ; j++) {
				std::cout << "\tlocation ";
				for (size_t k = 0; k < c.getLocationName(i, j).size(); k++)
					std::cout << c.getLocationName(i, j)[k] << " ";
				std::cout << std::endl;
				std::cout << "\t\troot: " << c.getLocationRoot(i, j) << std::endl;
				std::cout << "\t\tindex pages: ";
				for (size_t k = 0; k < c.getLocationIndexPages(i, j).size(); k++)
					std::cout << c.getLocationIndexPages(i, j)[k] << " ";
				std::cout << std::endl;
				std::cout << "\t\tautoindex: " << c.getLocationAutoIndex(i, j) << std::endl;
				std::cout << "\t\tallowed methods: ";
				for (size_t k = 0; k < c.getLocationMethods(i, j).size(); k++)
					std::cout << c.getLocationMethods(i, j)[k] << " ";
				std::cout << std::endl;
				std::cout << "\t\tCGI extensions: ";
				for (size_t k = 0; k < c.getLocationCGIExtensions(i, j).size(); k++)
					std::cout << c.getLocationCGIExtensions(i, j)[k] << " ";
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
