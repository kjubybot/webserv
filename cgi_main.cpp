#include <iostream>
#include "CGI.hpp"
#include <fstream>

int main()
{
	CGI cgi("/usr/bin/php", "./info.php");
	cgi.executeBaseCGI();
	std::ifstream input("cgi_response");
	std::string string;
	std::ofstream output("output.html");
	while(std::getline(input, string))
		output << string << std::endl;
	return (0);
}
