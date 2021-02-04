#include <iostream>
#include "includes/Config.hpp"

int main()
{
	try {
		Config c("../1.conf");
	}
	catch (const std::exception& ex) {
		std::cout << "exception: " << ex.what() << std::endl;
	}

}
